use std::cell::RefCell;
use std::rc::Rc;

// Ссылка на узел. Rc<RefCell<..>> нужен, потому что на последний узел
// указывают сразу двое: предыдущий узел и поле tail
pub type Link = Option<Rc<RefCell<Node>>>;

// Узел односвязного списка
pub struct Node {
    pub data: String,
    pub next: Link,
}

// Односвязный список, указатель на хвост даёт добавление в конец за O(1)
pub struct FList {
    pub head: Link,
    pub tail: Link,
}

pub fn new_list() -> FList {
    FList {
        head: None,
        tail: None,
    }
}

fn new_node(data: &str, next: Link) -> Rc<RefCell<Node>> {
    Rc::new(RefCell::new(Node {
        data: data.to_string(),
        next,
    }))
}

// Чтение от головы к хвосту
pub fn print_list(list: &FList) {
    if list.head.is_none() {
        println!("(empty)");
        return;
    }
    let mut current = list.head.clone();
    while let Some(node) = current {
        print!("{}", node.borrow().data);
        let next = node.borrow().next.clone();
        if next.is_some() {
            print!(" -> ");
        }
        current = next;
    }
    println!();
}

// Рекурсия: сначала выводится остаток списка, потом текущий узел
fn print_reverse_rec(link: &Link) {
    if let Some(node) = link {
        let node = node.borrow();
        print_reverse_rec(&node.next);
        if node.next.is_some() {
            print!(" <- ");
        }
        print!("{}", node.data);
    }
}

// Чтение от хвоста к голове
pub fn print_reverse(list: &FList) {
    if list.head.is_none() {
        println!("(empty)");
        return;
    }
    print_reverse_rec(&list.head);
    println!();
}

// Поиск первого узла с заданным значением
pub fn find_node(list: &FList, value: &str) -> Link {
    let mut current = list.head.clone();
    while let Some(node) = current {
        if node.borrow().data == value {
            return Some(node);
        }
        current = node.borrow().next.clone();
    }
    None
}

// Узел, за которым стоит первый узел со значением value (голова не проверяется)
fn find_prev(list: &FList, value: &str) -> Link {
    let mut current = list.head.clone();
    while let Some(node) = current {
        let next = node.borrow().next.clone();
        if let Some(next_node) = &next {
            if next_node.borrow().data == value {
                return Some(node);
            }
        }
        current = next;
    }
    None
}

pub fn get_by_index(list: &FList, index: usize) -> Option<String> {
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

pub fn add_head(list: &mut FList, data: &str) {
    let node = new_node(data, list.head.take());
    if list.tail.is_none() {
        list.tail = Some(Rc::clone(&node));
    }
    list.head = Some(node);
}

pub fn add_tail(list: &mut FList, data: &str) {
    let node = new_node(data, None);
    match list.tail.take() {
        Some(old_tail) => {
            old_tail.borrow_mut().next = Some(Rc::clone(&node));
        }
        None => {
            list.head = Some(Rc::clone(&node));
        }
    }
    list.tail = Some(node);
}

// Вставка перед target: нужен предыдущий узел, поэтому идём с головы
pub fn add_before(list: &mut FList, target: &str, data: &str) -> bool {
    let head = match list.head.clone() {
        Some(node) => node,
        None => return false,
    };
    if head.borrow().data == target {
        add_head(list, data);
        return true;
    }
    let prev = match find_prev(list, target) {
        Some(node) => node,
        None => return false,
    };
    let next = prev.borrow_mut().next.take();
    prev.borrow_mut().next = Some(new_node(data, next));
    true
}

pub fn add_after(list: &mut FList, target: &str, data: &str) -> bool {
    let current = match find_node(list, target) {
        Some(node) => node,
        None => return false,
    };
    let next = current.borrow_mut().next.take();
    let is_tail = next.is_none();
    let node = new_node(data, next);
    current.borrow_mut().next = Some(Rc::clone(&node));
    if is_tail {
        list.tail = Some(node);
    }
    true
}

pub fn delete_head(list: &mut FList) -> Option<String> {
    let head = list.head.take()?;
    list.head = head.borrow_mut().next.take();
    if list.head.is_none() {
        list.tail = None;
    }
    let data = head.borrow().data.clone();
    Some(data)
}

// Хвост известен, но для удаления нужен предпоследний узел — проход O(n)
pub fn delete_tail(list: &mut FList) -> Option<String> {
    let tail = list.tail.clone()?;
    let head = list.head.clone()?;
    if Rc::ptr_eq(&head, &tail) {
        return delete_head(list);
    }
    let mut prev = head;
    loop {
        let next = prev.borrow().next.clone()?;
        if Rc::ptr_eq(&next, &tail) {
            break;
        }
        prev = next;
    }
    prev.borrow_mut().next = None;
    list.tail = Some(prev);
    let data = tail.borrow().data.clone();
    Some(data)
}

// Удаление узла перед target: ищем узел, у которого через один стоит target
pub fn delete_before(list: &mut FList, target: &str) -> Option<String> {
    let head = list.head.clone()?;
    if head.borrow().data == target {
        return None;
    }
    let second = head.borrow().next.clone()?;
    if second.borrow().data == target {
        return delete_head(list);
    }
    let mut prev = head;
    loop {
        let middle = prev.borrow().next.clone()?;
        let after = middle.borrow().next.clone()?;
        if after.borrow().data == target {
            prev.borrow_mut().next = Some(after);
            let data = middle.borrow().data.clone();
            return Some(data);
        }
        prev = middle;
    }
}

pub fn delete_after(list: &mut FList, target: &str) -> Option<String> {
    let current = find_node(list, target)?;
    let removed = current.borrow_mut().next.take()?;
    let after = removed.borrow_mut().next.take();
    if after.is_none() {
        list.tail = Some(Rc::clone(&current));
    }
    current.borrow_mut().next = after;
    let data = removed.borrow().data.clone();
    Some(data)
}

pub fn delete_by_value(list: &mut FList, value: &str) -> bool {
    let head = match list.head.clone() {
        Some(node) => node,
        None => return false,
    };
    if head.borrow().data == value {
        delete_head(list);
        return true;
    }
    let prev = match find_prev(list, value) {
        Some(node) => node,
        None => return false,
    };
    let removed = prev.borrow_mut().next.take();
    let removed = match removed {
        Some(node) => node,
        None => return false,
    };
    let after = removed.borrow_mut().next.take();
    if after.is_none() {
        list.tail = Some(Rc::clone(&prev));
    }
    prev.borrow_mut().next = after;
    true
}
