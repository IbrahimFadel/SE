use flux_span::{Span, Spanned};

use crate::hir::Path;

use super::*;

pub(crate) enum PathResolutionResultKind {
    Type,
    Any,
    Use,
    Struct,
    Trait,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub(crate) enum ResolvePathError {
    EmptyPath { path_span: Span },
    UnresolvedPath { path: Spanned<Path>, segment: usize },
    PrivateModule { path: Spanned<Path>, segment: usize },
}

impl ResolvePathError {
    pub fn to_lower_error(
        &self,
        file_id: FileId,
        string_interner: &'static ThreadedRodeo,
        expected_kind: PathResolutionResultKind,
    ) -> LowerError {
        match self {
            Self::EmptyPath { path_span } => LowerError::CouldNotResolveEmptyPath {
                path: (),
                path_file_span: path_span.in_file(file_id),
            },
            Self::PrivateModule { path, segment } => {
                let spanned_segment =
                    Path::spanned_segment(path, *segment, string_interner).unwrap();
                LowerError::CannotAccessPrivatePathSegment {
                    path: path.inner.to_string(string_interner),
                    path_file_span: path.span.in_file(file_id),
                    erroneous_segment: string_interner.resolve(&spanned_segment.inner).to_string(),
                    erroneous_segment_file_span: spanned_segment.span.in_file(file_id),
                }
            }
            Self::UnresolvedPath { path, segment } => {
                let spanned_segment =
                    Path::spanned_segment(path, *segment, string_interner).unwrap();
                let path_file_span = path.span.in_file(file_id);
                let path = path.to_string(string_interner);
                let erroneous_segment_file_span = spanned_segment.span.in_file(file_id);
                let erroneous_segment = string_interner.resolve(&spanned_segment.inner).to_string();
                match expected_kind {
                    PathResolutionResultKind::Any => LowerError::CouldNotResolvePath {
                        path,
                        path_file_span,
                        erroneous_segment,
                        erroneous_segment_file_span,
                    },
                    PathResolutionResultKind::Type => LowerError::UnresolvedType {
                        ty: path,
                        ty_file_span: path_file_span,
                    },
                    PathResolutionResultKind::Use => LowerError::CouldNotResolvePath {
                        path,
                        path_file_span,
                        erroneous_segment,
                        erroneous_segment_file_span,
                    },
                    PathResolutionResultKind::Struct => LowerError::UnresolvedStruct {
                        strukt: path,
                        strukt_file_span: path_file_span,
                    },
                    PathResolutionResultKind::Trait => LowerError::UnresolvedTrait {
                        trt: path,
                        trt_file_span: path_file_span,
                    },
                }
            }
        }
    }
}

impl PackageData {
    pub(crate) fn resolve_path(
        &self,
        path: &Spanned<Path>,
        original_module_id: ModuleId,
        packages: &Arena<PackageData>,
    ) -> Result<(Option<PackageId>, Option<ModuleItemWithVis>), ResolvePathError> {
        self.def_map.resolve_path(
            &self.name,
            path,
            original_module_id,
            &self.dependencies,
            packages,
        )
    }
}

impl DefMap {
    pub(crate) fn resolve_path(
        &self,
        package_name: &Spur,
        path: &Spanned<Path>,
        original_module_id: ModuleId,
        dependencies: &[PackageDependency],
        packages: &Arena<PackageData>,
    ) -> Result<(Option<PackageId>, Option<ModuleItemWithVis>), ResolvePathError> {
        let mut segments = path.segments.iter().enumerate();
        let mut name = match segments.next() {
            Some((_, segment)) => segment,
            None => {
                return Err(ResolvePathError::EmptyPath {
                    path_span: path.span,
                })
            }
        };

        // If the path is absolute (aka, begins with package name, skip to first segment that needs to be resolved)
        if name == package_name {
            match segments.next() {
                Some((_, segment)) => name = segment,
                None => {
                    return Err(ResolvePathError::EmptyPath {
                        path_span: path.span,
                    })
                }
            };
        };

        let mut curr_per_ns = self.resolve_name_in_module(original_module_id, name);

        if curr_per_ns.is_none() {
            return self.try_resolve_in_dependency(
                path,
                original_module_id,
                dependencies,
                packages,
            );
        }

        for (i, segment) in segments {
            let (curr, m, vis) = match curr_per_ns {
                Some((curr, m, vis)) => (curr, m, vis),
                None => {
                    return Err(ResolvePathError::UnresolvedPath {
                        path: path.clone(),
                        segment: i,
                    })
                }
            };

            curr_per_ns = match curr {
                ModuleDefId::ModuleId(m) => self.resolve_name_in_module(m, segment),
                s => {
                    if vis == Visibility::Private {
                        return Err(ResolvePathError::PrivateModule {
                            path: path.clone(),
                            segment: i,
                        });
                    }
                    return Ok((None, Some((s, m, vis))));
                }
            };

            if let Some((_, _, vis)) = curr_per_ns {
                if vis == Visibility::Private {
                    return Err(ResolvePathError::PrivateModule {
                        path: path.clone(),
                        segment: i,
                    });
                }
            }
            if curr_per_ns.is_none() {
                return Err(ResolvePathError::UnresolvedPath {
                    path: path.clone(),
                    segment: i,
                });
            }
        }

        Ok((None, (curr_per_ns)))
    }

    fn resolve_name_in_module(&self, module: ModuleId, name: &Spur) -> Option<ModuleItemWithVis> {
        let from_scope = self[module].scope.get(name);
        let from_builtin = self.builtin_scope.get(name).copied();
        let from_prelude = || self.resolve_in_prelude(name);
        from_scope.or(from_builtin).or_else(from_prelude)
    }

    fn resolve_in_prelude(&self, name: &Spur) -> Option<ModuleItemWithVis> {
        self[self.prelude].scope.get(name)
    }

    fn try_resolve_in_dependency(
        &self,
        path: &Spanned<Path>,
        original_module_id: ModuleId,
        dependencies: &[PackageDependency],
        packages: &Arena<PackageData>,
    ) -> Result<(Option<PackageId>, Option<ModuleItemWithVis>), ResolvePathError> {
        for dep in dependencies {
            let package = &packages[dep.package_id];
            if &package.name == path.nth(0) {
                return package.resolve_path(path, original_module_id, packages);
                // return packagedef_map
                //     .resolve_path(path, def_map.root)
                //     .map(|(_, mod_item)| (Some(dep.clone()), mod_item));
            }
        }

        Err(ResolvePathError::UnresolvedPath {
            path: path.clone(),
            segment: 0,
        })
    }
}
