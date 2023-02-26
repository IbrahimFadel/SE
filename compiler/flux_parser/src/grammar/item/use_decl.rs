use crate::{grammar::name, marker::CompletedMarker};

use super::*;

pub(crate) fn use_decl(p: &mut Parser, visibility: CompletedMarker) {
    let m = visibility.precede(p);
    p.bump(TokenKind::Use);

    let path_m = p.start();
    p.expect(TokenKind::Ident);
    while p.at(TokenKind::DoubleColon) {
        p.bump(TokenKind::DoubleColon);
        if !p.expect(TokenKind::Ident) {
            break;
        }
    }
    path_m.complete(p, SyntaxKind::Path);

    if p.eat(TokenKind::As) {
        name(p, TokenSet::new(&[TokenKind::SemiColon]));
    }
    p.expect(TokenKind::SemiColon);
    m.complete(p, SyntaxKind::UseDecl);
}
