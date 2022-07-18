use flux_lexer::TokenKind;
use num_derive::{FromPrimitive, ToPrimitive};
use num_traits::{FromPrimitive, ToPrimitive};

#[derive(Debug, Copy, Clone, PartialEq, FromPrimitive, ToPrimitive, Hash, Eq, PartialOrd, Ord)]
pub enum SyntaxKind {
	Root,
	BinExpr,
	PrefixExpr,
	ParenExpr,
	VarDecl,
	FnDecl,
	GenericList,
	FnParams,
	FnParam,
	ThisParam,
	TypeExpr,
	PrimitiveType,
	BlockExpr,
	ExprStmt,
	IdentExpr,
	IntExpr,
	FloatExpr,
	IfExpr,
	TypeDecl,
	StructType,
	StructTypeField,
	TraitDecl,
	TraitMethod,
	IdentType,
	CallExpr,
	ReturnStmt,
	ModDecl,
	UseDecl,
	PathExpr,
	StructExpr,
	StructExprField,
	TupleExpr,
	TupleType,
	ApplyDecl,
	ApplyBlock,

	Whitespace,
	Comment,
	ModKw,
	UseKw,
	PubKw,
	FnKw,
	TypeKw,
	ApplyKw,
	ToKw,
	WhereKw,
	MutKw,
	IfKw,
	ElseKw,
	StructKw,
	TraitKw,
	LetKw,
	ReturnKw,
	INKw,
	UNKw,
	F64Kw,
	F32Kw,
	BoolKw,
	Ident,

	Comma,
	CmpEq,
	CmpNeq,
	CmpLt,
	CmpGt,
	CmpLte,
	CmpGte,
	Colon,
	DoubleColon,
	Plus,
	Minus,
	Star,
	Slash,
	Arrow,
	FatArrow,
	LParen,
	RParen,
	LBrace,
	RBrace,
	Eq,
	SemiColon,
	Error,
}

impl From<TokenKind> for SyntaxKind {
	fn from(token_kind: TokenKind) -> Self {
		match token_kind {
			TokenKind::Root => SyntaxKind::Root,
			TokenKind::Whitespace => SyntaxKind::Whitespace,
			TokenKind::Comment => SyntaxKind::Comment,
			TokenKind::Ident => SyntaxKind::Ident,
			TokenKind::IntLit => SyntaxKind::IntExpr,
			TokenKind::FloatLit => SyntaxKind::FloatExpr,
			TokenKind::ModKw => SyntaxKind::ModKw,
			TokenKind::UseKw => SyntaxKind::UseKw,
			TokenKind::PubKw => SyntaxKind::PubKw,
			TokenKind::FnKw => SyntaxKind::FnKw,
			TokenKind::TypeKw => SyntaxKind::TypeKw,
			TokenKind::ApplyKw => SyntaxKind::ApplyDecl,
			TokenKind::ToKw => SyntaxKind::ToKw,
			TokenKind::WhereKw => SyntaxKind::WhereKw,
			TokenKind::MutKw => SyntaxKind::MutKw,
			TokenKind::IfKw => SyntaxKind::IfKw,
			TokenKind::ElseKw => SyntaxKind::ElseKw,
			TokenKind::StructKw => SyntaxKind::StructKw,
			TokenKind::TraitKw => SyntaxKind::TraitKw,
			TokenKind::LetKw => SyntaxKind::LetKw,
			TokenKind::ReturnKw => SyntaxKind::ReturnKw,
			TokenKind::INKw => SyntaxKind::INKw,
			TokenKind::UNKw => SyntaxKind::UNKw,
			TokenKind::F64Kw => SyntaxKind::F64Kw,
			TokenKind::F32Kw => SyntaxKind::F32Kw,
			TokenKind::BoolKw => SyntaxKind::BoolKw,
			TokenKind::Comma => SyntaxKind::Comma,
			TokenKind::CmpEq => SyntaxKind::CmpEq,
			TokenKind::CmpNeq => SyntaxKind::CmpNeq,
			TokenKind::CmpLt => SyntaxKind::CmpLt,
			TokenKind::CmpGt => SyntaxKind::CmpGt,
			TokenKind::CmpLte => SyntaxKind::CmpLte,
			TokenKind::CmpGte => SyntaxKind::CmpGte,
			TokenKind::Colon => SyntaxKind::Colon,
			TokenKind::DoubleColon => SyntaxKind::DoubleColon,
			TokenKind::Plus => SyntaxKind::Plus,
			TokenKind::Minus => SyntaxKind::Minus,
			TokenKind::Star => SyntaxKind::Star,
			TokenKind::Slash => SyntaxKind::Slash,
			TokenKind::Arrow => SyntaxKind::Arrow,
			TokenKind::FatArrow => SyntaxKind::FatArrow,
			TokenKind::LParen => SyntaxKind::LParen,
			TokenKind::RParen => SyntaxKind::RParen,
			TokenKind::LBrace => SyntaxKind::RBrace,
			TokenKind::RBrace => SyntaxKind::RBrace,
			TokenKind::Eq => SyntaxKind::Eq,
			TokenKind::SemiColon => SyntaxKind::SemiColon,
			TokenKind::Error => SyntaxKind::Error,
		}
	}
}

#[macro_export]
macro_rules! S {
	["+"] => { SyntaxKind::Plus };
	["-"] => { SyntaxKind::Minus };
	["*"] => { SyntaxKind::Star };
	["/"] => { SyntaxKind::Slash };
	["int_number"] => { SyntaxKind::INKw };
	["ident"] => { SyntaxKind::Ident };
}

#[derive(Debug, Copy, Clone, Ord, PartialOrd, Eq, PartialEq, Hash)]
pub enum PILanguage {}

impl rowan::Language for PILanguage {
	type Kind = SyntaxKind;

	fn kind_from_raw(raw: rowan::SyntaxKind) -> Self::Kind {
		Self::Kind::from_u16(raw.0).unwrap()
	}

	fn kind_to_raw(kind: Self::Kind) -> rowan::SyntaxKind {
		rowan::SyntaxKind(kind.to_u16().unwrap())
	}
}

pub type SyntaxNode = rowan::SyntaxNode<PILanguage>;
pub type SyntaxToken = rowan::SyntaxToken<PILanguage>;
pub type SyntaxElement = rowan::SyntaxElement<PILanguage>;
