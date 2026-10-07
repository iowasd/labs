use std::fs;

use crate::array::{self, Array};
use crate::dlist::{self, DList};
use crate::flist::{self, FList};
use crate::queue::{self, Queue};
use crate::stack::{self, Stack};

// ---------------- Вспомогательные функции ----------------

// Разбивает строку на слова по пробелам
fn split_words(text: &str) -> Array {
    let mut words = array::init(4);
    for word in text.split_whitespace() {
        array::push(&mut words, word);
    }
    words
}

// Дописывает слово в строку через пробел
fn append_word(text: &mut String, word: &str) {
    if !text.is_empty() {
        text.push(' ');
    }
    text.push_str(word);
}

fn parse_index(text: &str) -> Option<usize> {
    text.parse::<usize>().ok()
}

fn print_result(text: &str) {
    println!("-> {}", text);
}

fn print_error(text: &str) {
    println!("-> ERROR: {}", text);
}

// Печатает удалённое значение или ошибку, возвращает true, если удаление было
fn report_removed(removed: Option<String>, error: &str) -> bool {
    match removed {
        Some(value) => {
            print_result(&value);
            true
        }
        None => {
            print_error(error);
            false
        }
    }
}

fn print_bool(value: bool) {
    if value {
        print_result("TRUE");
    } else {
        print_result("FALSE");
    }
}

// ---------------- Работа с файлом ----------------

// Строка файла: <тип> <имя> <элемент1> <элемент2> ...
fn load_file(file_name: &str) -> Array {
    let mut lines = array::init(8);
    if let Ok(content) = fs::read_to_string(file_name) {
        for line in content.lines() {
            if !line.is_empty() {
                array::push(&mut lines, line);
            }
        }
    }
    lines
}

fn save_file(file_name: &str, lines: &Array) -> bool {
    let mut content = String::new();
    for i in 0..lines.size {
        content.push_str(&lines.data[i]);
        content.push('\n');
    }
    fs::write(file_name, content).is_ok()
}

// Индекс строки со структурой name
fn find_line(lines: &Array, name: &str) -> Option<usize> {
    for i in 0..lines.size {
        let mut parts = lines.data[i].split_whitespace();
        parts.next();
        if parts.next() == Some(name) {
            return Some(i);
        }
    }
    None
}

// Возвращает тип структуры, элементы складывает в items
fn parse_line(line: &str, items: &mut Array) -> String {
    let mut parts = line.split_whitespace();
    let kind = parts.next().unwrap_or("").to_string();
    parts.next();
    for word in parts {
        array::push(items, word);
    }
    kind
}

// Тип структуры по первой букве команды
fn command_type(command: &str) -> String {
    if command == "ISMEMBER" {
        return "T".to_string();
    }
    match command.chars().next() {
        Some(first) if "MFLQST".contains(first) => first.to_string(),
        _ => String::new(),
    }
}

// ---------------- Массив (M) ----------------

// Возвращает true, если массив изменился и его нужно сохранить
fn array_command(command: &str, words: &Array, arr: &mut Array) -> bool {
    let count = words.size;
    let w = &words.data;

    if command == "MCREATE" && count == 2 {
        print_result(&w[1]);
        return true;
    }
    if command == "MPUSH" && count == 3 {
        array::push(arr, &w[2]);
        print_result(&w[2]);
        return true;
    }
    if command == "MINSERT" && count == 4 {
        let inserted = match parse_index(&w[2]) {
            Some(index) => array::insert(arr, index, &w[3]),
            None => false,
        };
        if !inserted {
            print_error("invalid index");
            return false;
        }
        print_result(&w[3]);
        return true;
    }
    if command == "MGET" && count == 3 {
        match parse_index(&w[2]).and_then(|index| array::get(arr, index)) {
            Some(value) => print_result(&value),
            None => print_error("invalid index"),
        }
        return false;
    }
    if command == "MSET" && count == 4 {
        let replaced = match parse_index(&w[2]) {
            Some(index) => array::set(arr, index, &w[3]),
            None => false,
        };
        if !replaced {
            print_error("invalid index");
            return false;
        }
        print_result(&w[3]);
        return true;
    }
    if command == "MDEL" && count == 3 {
        let removed = parse_index(&w[2]).and_then(|index| array::delete(arr, index));
        return report_removed(removed, "invalid index");
    }
    if command == "MLEN" && count == 2 {
        print_result(&array::length(arr).to_string());
        return false;
    }
    if command == "PRINT" {
        print!("-> ");
        array::print(arr);
        return false;
    }

    print_error("unknown command or wrong arguments");
    false
}

fn run_array(command: &str, words: &Array, items: &Array, saved: &mut String) -> bool {
    let mut arr = array::init(items.size);
    for i in 0..items.size {
        array::push(&mut arr, &items.data[i]);
    }

    let changed = array_command(command, words, &mut arr);
    if changed {
        for i in 0..arr.size {
            append_word(saved, &arr.data[i]);
        }
    }
    changed
}

// ---------------- Односвязный список (F) ----------------

fn flist_command(command: &str, words: &Array, list: &mut FList) -> bool {
    let count = words.size;
    let w = &words.data;

    // FPUSH без уточнения — добавление в хвост, как в таблице задания
    if (command == "FPUSH" || command == "FPUSHTAIL") && count == 3 {
        flist::add_tail(list, &w[2]);
        print_result(&w[2]);
        return true;
    }
    if command == "FPUSHHEAD" && count == 3 {
        flist::add_head(list, &w[2]);
        print_result(&w[2]);
        return true;
    }
    if command == "FPUSHBEFORE" && count == 4 {
        if !flist::add_before(list, &w[2], &w[3]) {
            print_error(&format!("element {} not found", w[2]));
            return false;
        }
        print_result(&w[3]);
        return true;
    }
    if command == "FPUSHAFTER" && count == 4 {
        if !flist::add_after(list, &w[2], &w[3]) {
            print_error(&format!("element {} not found", w[2]));
            return false;
        }
        print_result(&w[3]);
        return true;
    }
    if command == "FDELHEAD" && count == 2 {
        return report_removed(flist::delete_head(list), "list is empty");
    }
    if command == "FDELTAIL" && count == 2 {
        return report_removed(flist::delete_tail(list), "list is empty");
    }
    if command == "FDELBEFORE" && count == 3 {
        let error = format!("nothing to delete before {}", w[2]);
        return report_removed(flist::delete_before(list, &w[2]), &error);
    }
    if command == "FDELAFTER" && count == 3 {
        let error = format!("nothing to delete after {}", w[2]);
        return report_removed(flist::delete_after(list, &w[2]), &error);
    }
    // FDEL — удаление по значению
    if command == "FDEL" && count == 3 {
        if !flist::delete_by_value(list, &w[2]) {
            print_error(&format!("element {} not found", w[2]));
            return false;
        }
        print_result(&w[2]);
        return true;
    }
    if command == "FGET" && count == 3 {
        match parse_index(&w[2]).and_then(|index| flist::get_by_index(list, index)) {
            Some(value) => print_result(&value),
            None => print_error("invalid index"),
        }
        return false;
    }
    if command == "FFIND" && count == 3 {
        print_bool(flist::find_node(list, &w[2]).is_some());
        return false;
    }
    if command == "FPRINTREV" && count == 2 {
        print!("-> ");
        flist::print_reverse(list);
        return false;
    }
    if command == "PRINT" {
        print!("-> ");
        flist::print_list(list);
        return false;
    }

    print_error("unknown command or wrong arguments");
    false
}

fn run_flist(command: &str, words: &Array, items: &Array, saved: &mut String) -> bool {
    let mut list = flist::new_list();
    for i in 0..items.size {
        flist::add_tail(&mut list, &items.data[i]);
    }

    let changed = flist_command(command, words, &mut list);
    if changed {
        let mut current = list.head.clone();
        while let Some(node) = current {
            append_word(saved, &node.borrow().data);
            current = node.borrow().next.clone();
        }
    }
    changed
}

// ---------------- Двусвязный список (L) ----------------

fn dlist_command(command: &str, words: &Array, list: &mut DList) -> bool {
    let count = words.size;
    let w = &words.data;

    // LPUSH без уточнения — добавление в хвост, как в таблице задания
    if (command == "LPUSH" || command == "LPUSHTAIL") && count == 3 {
        dlist::add_tail(list, &w[2]);
        print_result(&w[2]);
        return true;
    }
    if command == "LPUSHHEAD" && count == 3 {
        dlist::add_head(list, &w[2]);
        print_result(&w[2]);
        return true;
    }
    if command == "LPUSHBEFORE" && count == 4 {
        if !dlist::add_before(list, &w[2], &w[3]) {
            print_error(&format!("element {} not found", w[2]));
            return false;
        }
        print_result(&w[3]);
        return true;
    }
    if command == "LPUSHAFTER" && count == 4 {
        if !dlist::add_after(list, &w[2], &w[3]) {
            print_error(&format!("element {} not found", w[2]));
            return false;
        }
        print_result(&w[3]);
        return true;
    }
    if command == "LDELHEAD" && count == 2 {
        return report_removed(dlist::delete_head(list), "list is empty");
    }
    if command == "LDELTAIL" && count == 2 {
        return report_removed(dlist::delete_tail(list), "list is empty");
    }
    if command == "LDELBEFORE" && count == 3 {
        let error = format!("nothing to delete before {}", w[2]);
        return report_removed(dlist::delete_before(list, &w[2]), &error);
    }
    if command == "LDELAFTER" && count == 3 {
        let error = format!("nothing to delete after {}", w[2]);
        return report_removed(dlist::delete_after(list, &w[2]), &error);
    }
    // LDEL — удаление по значению
    if command == "LDEL" && count == 3 {
        if !dlist::delete_by_value(list, &w[2]) {
            print_error(&format!("element {} not found", w[2]));
            return false;
        }
        print_result(&w[2]);
        return true;
    }
    if command == "LGET" && count == 3 {
        match parse_index(&w[2]).and_then(|index| dlist::get_by_index(list, index)) {
            Some(value) => print_result(&value),
            None => print_error("invalid index"),
        }
        return false;
    }
    if command == "LFIND" && count == 3 {
        print_bool(dlist::find_node(list, &w[2]).is_some());
        return false;
    }
    if command == "LPRINTREV" && count == 2 {
        print!("-> ");
        dlist::print_reverse(list);
        return false;
    }
    if command == "PRINT" {
        print!("-> ");
        dlist::print_list(list);
        return false;
    }

    print_error("unknown command or wrong arguments");
    false
}

fn run_dlist(command: &str, words: &Array, items: &Array, saved: &mut String) -> bool {
    let mut list = dlist::new_list();
    for i in 0..items.size {
        dlist::add_tail(&mut list, &items.data[i]);
    }

    let changed = dlist_command(command, words, &mut list);
    if changed {
        let mut current = list.head.clone();
        while let Some(node) = current {
            append_word(saved, &node.borrow().data);
            current = node.borrow().next.clone();
        }
    }
    changed
}

// ---------------- Стек (S) ----------------

fn stack_command(command: &str, words: &Array, stack: &mut Stack) -> bool {
    let count = words.size;
    let w = &words.data;

    if command == "SPUSH" && count == 3 {
        stack::push(stack, &w[2]);
        print_result(&w[2]);
        return true;
    }
    if command == "SPOP" && count == 2 {
        return report_removed(stack::pop(stack), "stack is empty");
    }
    if command == "PRINT" {
        print!("-> ");
        stack::print_stack(stack);
        return false;
    }

    print_error("unknown command or wrong arguments");
    false
}

fn run_stack(command: &str, words: &Array, items: &Array, saved: &mut String) -> bool {
    let mut stack = stack::new_stack();
    // В файле стек записан от вершины ко дну, поэтому кладём с конца
    for i in (0..items.size).rev() {
        stack::push(&mut stack, &items.data[i]);
    }

    let changed = stack_command(command, words, &mut stack);
    if changed {
        let mut current = stack.top.as_ref();
        while let Some(node) = current {
            append_word(saved, &node.data);
            current = node.next.as_ref();
        }
    }
    changed
}

// ---------------- Очередь (Q) ----------------

fn queue_command(command: &str, words: &Array, queue: &mut Queue) -> bool {
    let count = words.size;
    let w = &words.data;

    if command == "QPUSH" && count == 3 {
        queue::push(queue, &w[2]);
        print_result(&w[2]);
        return true;
    }
    if command == "QPOP" && count == 2 {
        return report_removed(queue::pop(queue), "queue is empty");
    }
    if command == "PRINT" {
        print!("-> ");
        queue::print_queue(queue);
        return false;
    }

    print_error("unknown command or wrong arguments");
    false
}

fn run_queue(command: &str, words: &Array, items: &Array, saved: &mut String) -> bool {
    let mut queue = queue::new_queue();
    for i in 0..items.size {
        queue::push(&mut queue, &items.data[i]);
    }

    let changed = queue_command(command, words, &mut queue);
    if changed {
        let mut current = queue.head.clone();
        while let Some(node) = current {
            append_word(saved, &node.borrow().data);
            current = node.borrow().next.clone();
        }
    }
    changed
}

// ---------------- Выполнение запроса ----------------

// Загружает файл, выполняет запрос и сохраняет файл, если данные изменились
pub fn execute_query(file_name: &str, query: &str) {
    let words = split_words(query);
    if words.size < 2 {
        print_error("query format: <COMMAND> <name> [args]");
        return;
    }

    let command = words.data[0].to_uppercase();
    let name = words.data[1].clone();

    let mut lines = load_file(file_name);
    let mut items = array::init(8);
    let index = find_line(&lines, &name);
    let mut stored_type = String::new();
    if let Some(i) = index {
        stored_type = parse_line(&lines.data[i], &mut items);
    }

    // PRINT работает с любой структурой, тип берётся из файла
    let mut kind = command_type(&command);
    if command == "PRINT" {
        kind = stored_type.clone();
    }

    if command == "PRINT" && index.is_none() {
        print_error(&format!("structure {} not found", name));
        return;
    }
    if kind == "T" {
        print_error("trees are implemented only in the C++ version");
        return;
    }
    if kind.is_empty() {
        print_error(&format!("unknown command {}", command));
        return;
    }
    if command == "MCREATE" && index.is_some() {
        print_error(&format!("structure {} already exists", name));
        return;
    }
    if index.is_some() && stored_type != kind {
        print_error(&format!(
            "name {} is already used by structure of type {}",
            name, stored_type
        ));
        return;
    }

    let mut saved = String::new();
    let changed = match kind.as_str() {
        "M" => run_array(&command, &words, &items, &mut saved),
        "F" => run_flist(&command, &words, &items, &mut saved),
        "L" => run_dlist(&command, &words, &items, &mut saved),
        "S" => run_stack(&command, &words, &items, &mut saved),
        "Q" => run_queue(&command, &words, &items, &mut saved),
        _ => {
            print_error(&format!("unknown structure type {} in file", kind));
            false
        }
    };

    // Файл перезаписывается только при изменении состава элементов
    if changed {
        let mut line = format!("{} {}", kind, name);
        if !saved.is_empty() {
            line.push(' ');
            line.push_str(&saved);
        }
        match index {
            Some(i) => {
                array::set(&mut lines, i, &line);
            }
            None => {
                array::push(&mut lines, &line);
            }
        }
        if !save_file(file_name, &lines) {
            print_error(&format!("cannot write file {}", file_name));
        }
    }
}
