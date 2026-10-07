// Узел стека: стек построен на односвязном списке, вершина — голова
pub struct SNode {
    pub data: String,
    pub next: Option<Box<SNode>>,
}

pub struct Stack {
    pub top: Option<Box<SNode>>,
}

pub fn new_stack() -> Stack {
    Stack { top: None }
}

pub fn push(stack: &mut Stack, value: &str) {
    let node = Box::new(SNode {
        data: value.to_string(),
        next: stack.top.take(),
    });
    stack.top = Some(node);
}

// Снимает вершину и возвращает её значение
pub fn pop(stack: &mut Stack) -> Option<String> {
    let node = stack.top.take()?;
    let SNode { data, next } = *node;
    stack.top = next;
    Some(data)
}

// Чтение от вершины ко дну
pub fn print_stack(stack: &Stack) {
    if stack.top.is_none() {
        println!("(empty)");
        return;
    }
    print!("(top)");
    let mut current = stack.top.as_ref();
    while let Some(node) = current {
        print!(" {}", node.data);
        current = node.next.as_ref();
    }
    println!();
}
