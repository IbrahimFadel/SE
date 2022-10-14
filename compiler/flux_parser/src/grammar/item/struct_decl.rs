use flux_lexer::TokenKind;
use flux_syntax::SyntaxKind;

use crate::{
    grammar::{
        generic_params::{opt_generic_param_list, opt_where_clause},
        name,
        r#type::type_,
    },
    marker::CompletedMarker,
    parser::Parser,
    token_set::TokenSet,
};

use super::ITEM_RECOVERY_SET;

pub(super) fn struct_decl(p: &mut Parser, visibility: CompletedMarker) {
    let m = visibility.precede(p);
    p.bump(TokenKind::Struct);
    name(p, ITEM_RECOVERY_SET);
    opt_generic_param_list(p);
    opt_where_clause(p, TokenSet::new(&[TokenKind::LBrace]));
    if !p.eat(TokenKind::LBrace) {
        p.error("`{` in struct declaration");
    }
    while p.loop_safe_not_at(TokenKind::RBrace) {
        struct_decl_field(p);
        if !p.eat(TokenKind::Comma) {
            break;
        }
    }
    p.expect(TokenKind::RBrace);
    m.complete(p, SyntaxKind::StructDecl);
}

fn struct_decl_field(p: &mut Parser) {
    let m = p.start();
    p.expect(TokenKind::Ident);
    type_(p);
    m.complete(p, SyntaxKind::StructDeclField);
}
