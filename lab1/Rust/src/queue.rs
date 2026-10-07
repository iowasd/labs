use std::cell::RefCell;
use std::rc::Rc;

pub type Link = Option<Rc<RefCell<QNode>>>;

// Узел очереди
pub struct QNode {
    pub data: String,
    pub next: Link,
}

// Очередь: добавление в хвост, извлечение из головы
pub struct Queue {
    pub head: Link,
    pub tail: Link,
}

pub fn new_queue() -> Queue {
    Queue {
        head: None,
        tail: None,
    }
}

// Указатель на хвост даёт добавление за O(1)
pub fn push(queue: &mut Queue, value: &str) {
    let node = Rc::new(RefCell::new(QNode {
        data: value.to_string(),
        next: None,
    }));
    match queue.tail.take() {
        Some(old_tail) => {
            old_tail.borrow_mut().next = Some(Rc::clone(&node));
        }
        None => {
            queue.head = Some(Rc::clone(&node));
        }
    }
    queue.tail = Some(node);
}

// Извлекает голову и возвращает её значение
pub fn pop(queue: &mut Queue) -> Option<String> {
    let head = queue.head.take()?;
    queue.head = head.borrow_mut().next.take();
    if queue.head.is_none() {
        queue.tail = None;
    }
    let data = head.borrow().data.clone();
    Some(data)
}

// Чтение от головы к хвосту
pub fn print_queue(queue: &Queue) {
    if queue.head.is_none() {
        println!("(empty)");
        return;
    }
    print!("(head)");
    let mut current = queue.head.clone();
    while let Some(node) = current {
        print!(" {}", node.borrow().data);
        current = node.borrow().next.clone();
    }
    println!(" (tail)");
}
