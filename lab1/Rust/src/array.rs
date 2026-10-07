// Динамический массив строк: память выделяется блоком фиксированной ёмкости,
// при заполнении ёмкость увеличивается в 2 раза
pub struct Array {
    pub data: Box<[String]>,
    pub size: usize,
    pub capacity: usize,
}

fn allocate(capacity: usize) -> Box<[String]> {
    (0..capacity).map(|_| String::new()).collect()
}

pub fn init(capacity: usize) -> Array {
    let capacity = if capacity < 1 { 1 } else { capacity };
    Array {
        data: allocate(capacity),
        size: 0,
        capacity,
    }
}

// Перенос элементов в новый блок удвоенной ёмкости
fn grow(arr: &mut Array) {
    let new_capacity = arr.capacity * 2;
    let mut new_data = allocate(new_capacity);
    for i in 0..arr.size {
        new_data[i] = std::mem::take(&mut arr.data[i]);
    }
    arr.data = new_data;
    arr.capacity = new_capacity;
}

// Добавление в конец
pub fn push(arr: &mut Array, value: &str) {
    if arr.size == arr.capacity {
        grow(arr);
    }
    arr.data[arr.size] = value.to_string();
    arr.size += 1;
}

// Добавление по индексу: элементы правее сдвигаются на одну позицию
pub fn insert(arr: &mut Array, index: usize, value: &str) -> bool {
    if index > arr.size {
        return false;
    }
    if arr.size == arr.capacity {
        grow(arr);
    }
    let mut i = arr.size;
    while i > index {
        arr.data[i] = std::mem::take(&mut arr.data[i - 1]);
        i -= 1;
    }
    arr.data[index] = value.to_string();
    arr.size += 1;
    true
}

pub fn get(arr: &Array, index: usize) -> Option<String> {
    if index >= arr.size {
        return None;
    }
    Some(arr.data[index].clone())
}

pub fn set(arr: &mut Array, index: usize, value: &str) -> bool {
    if index >= arr.size {
        return false;
    }
    arr.data[index] = value.to_string();
    true
}

// Удаление по индексу: элементы правее сдвигаются влево
pub fn delete(arr: &mut Array, index: usize) -> Option<String> {
    if index >= arr.size {
        return None;
    }
    let removed = std::mem::take(&mut arr.data[index]);
    for i in index..arr.size - 1 {
        arr.data[i] = std::mem::take(&mut arr.data[i + 1]);
    }
    arr.size -= 1;
    Some(removed)
}

pub fn length(arr: &Array) -> usize {
    arr.size
}

pub fn print(arr: &Array) {
    print!("[");
    for i in 0..arr.size {
        if i > 0 {
            print!(", ");
        }
        print!("{}", arr.data[i]);
    }
    println!("]");
}
