use std::cell::RefCell;
use std::rc::{Rc, Weak};

pub type Link = Option<Rc<RefCell<DNode>>>;

// Узел двусвязного списка. prev хранится как Weak (слабая ссылка),
// чтобы соседние узлы не удерживали друг друга в памяти по кругу
pub struct DNode {
    pub data: String,
    pub next: Link,
    pub prev: Option<Weak<RefCell<DNode>>>,
}

// Двусвязный список с указателями на голову и хвост
pub struct DList {
    pub head: Link,
    pub tail: Link,
}

pub fn new_list() -> DList {
    DList {
        head: None,
        tail: None,
    }
}

fn new_node(data: &str) -> Rc<RefCell<DNode>> {
    Rc::new(RefCell::new(DNode {
        data: data.to_string(),
        next: None,
        prev: None,
    }))
}

// Предыдущий узел: слабая ссылка превращается в обычную через upgrade
fn prev_of(node: &Rc<RefCell<DNode>>) -> Link {
    let prev = node.borrow().prev.clone();
    prev.and_then(|weak| weak.upgrade())
}

// Чтение от головы к хвосту
pub fn print_list(list: &DList) {
    if list.head.is_none() {
        println!("(empty)");
        return;
    }
    let mut current = list.head.clone();
    while let Some(node) = current {
        print!("{}", node.borrow().data);
        let next = node.borrow().next.clone();
        if next.is_some() {
            print!(" <-> ");
        }
        current = next;
    }
    println!();
}

// Чтение от хвоста к голове по ссылкам prev
pub fn print_reverse(list: &DList) {
    if list.tail.is_none() {
        println!("(empty)");
        return;
    }
    let mut current = list.tail.clone();
    while let Some(node) = current {
        print!("{}", node.borrow().data);
        let prev = prev_of(&node);
        if prev.is_some() {
            print!(" <-> ");
        }
        current = prev;
    }
    println!();
}

// Поиск первого узла с заданным значением
pub fn find_node(list: &DList, value: &str) -> Link {
    let mut current = list.head.clone();
    while let Some(node) = current {
        if node.borrow().data == value {
            return Some(node);
        }
        current = node.borrow().next.clone();
    }
    None
}

pub fn get_by_index(list: &DList, index: usize) -> Option<String> {
    let mut current = list.head.clone();
    let mut i = 0;
    while let Some(node) = current {
        if i == index {
            let data = node.borrow().data.clone();
            return Some(data);
        }
        current = node.borrow().next.clone();
        i += 1;
    }
    None
}

pub fn add_head(list: &mut DList, data: &str) {
    let node = new_node(data);
    match list.head.take() {
        Some(old_head) => {
            old_head.borrow_mut().prev = Some(Rc::downgrade(&node));
            node.borrow_mut().next = Some(old_head);
        }
        None => {
            list.tail = Some(Rc::clone(&node));
        }
    }
    list.head = Some(node);
}

pub fn add_tail(list: &mut DList, data: &str) {
    let node = new_node(data);
    match list.tail.take() {
        Some(old_tail) => {
            node.borrow_mut().prev = Some(Rc::downgrade(&old_tail));
            old_tail.borrow_mut().next = Some(Rc::clone(&node));
        }
        None => {
            list.head = Some(Rc::clone(&node));
        }
    }
    list.tail = Some(node);
}

// Благодаря prev предыдущий узел известен сразу после поиска target
pub fn add_before(list: &mut DList, target: &str, data: &str) -> bool {
    let current = match find_node(list, target) {
        Some(node) => node,
        None => return false,
    };
    let node = new_node(data);
    let prev = prev_of(&current);
    node.borrow_mut().next = Some(Rc::clone(&current));
    current.borrow_mut().prev = Some(Rc::downgrade(&node));
    match prev {
        Some(prev_node) => {
            node.borrow_mut().prev = Some(Rc::downgrade(&prev_node));
            prev_node.borrow_mut().next = Some(node);
        }
        None => {
            list.head = Some(node);
        }
    }
    true
}

pub fn add_after(list: &mut DList, target: &str, data: &str) -> bool {
    let current = match find_node(list, target) {
        Some(node) => node,
        None => return false,
    };
    let node = new_node(data);
    let next = current.borrow_mut().next.take();
    node.borrow_mut().prev = Some(Rc::downgrade(&current));
    match next {
        Some(next_node) => {
            next_node.borrow_mut().prev = Some(Rc::downgrade(&node));
            node.borrow_mut().next = Some(next_node);
        }
        None => {
            list.tail = Some(Rc::clone(&node));
        }
    }
    current.borrow_mut().next = Some(node);
    true
}

// Отвязывает узел от соседей за O(1) и возвращает его значение
fn remove_node(list: &mut DList, node: &Rc<RefCell<DNode>>) -> String {
    let prev = prev_of(node);
    let next = node.borrow_mut().next.take();
    match &prev {
        Some(prev_node) => {
            prev_node.borrow_mut().next = next.clone();
        }
        None => {
            list.head = next.clone();
        }
    }
    match &next {
        Some(next_node) => {
            next_node.borrow_mut().prev = prev.as_ref().map(Rc::downgrade);
        }
        None => {
            list.tail = prev.clone();
        }
    }
    node.borrow_mut().prev = None;
    let data = node.borrow().data.clone();
    data
}

pub fn delete_head(list: &mut DList) -> Option<String> {
    let head = list.head.clone()?;
    Some(remove_node(list, &head))
}

pub fn delete_tail(list: &mut DList) -> Option<String> {
    let tail = list.tail.clone()?;
    Some(remove_node(list, &tail))
}

pub fn delete_before(list: &mut DList, target: &str) -> Option<String> {
    let current = find_node(list, target)?;
    let prev = prev_of(&current)?;
    Some(remove_node(list, &prev))
}

pub fn delete_after(list: &mut DList, target: &str) -> Option<String> {
    let current = find_node(list, target)?;
    let next = current.borrow().next.clone()?;
    Some(remove_node(list, &next))
}

pub fn delete_by_value(list: &mut DList, value: &str) -> bool {
    match find_node(list, value) {
        Some(node) => {
            remove_node(list, &node);
            true
        }
        None => false,
    }
}
