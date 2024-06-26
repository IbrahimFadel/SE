use std::collections::HashMap;

use flux_span::Word;

use crate::{hir::Visibility, item::ItemId};

#[derive(Debug, Default)]
pub(crate) struct ItemScope {
    pub items: HashMap<Word, (Visibility, ItemId)>,
}

impl ItemScope {
    pub fn declare(&mut self, name: Word, visibility: Visibility, item_id: ItemId) {
        self.items.insert(name, (visibility, item_id));
    }
}
