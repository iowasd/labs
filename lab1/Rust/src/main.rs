mod array;
mod database;
mod dlist;
mod flist;
mod queue;
mod stack;

use std::env;
use std::process;

fn main() {
    let mut file_name = String::new();
    let mut query = String::new();

    let mut args = env::args().skip(1);
    while let Some(arg) = args.next() {
        if arg == "--file" {
            if let Some(value) = args.next() {
                file_name = value;
            }
        } else if arg == "--query" {
            if let Some(value) = args.next() {
                query = value;
            }
        }
    }

    if file_name.is_empty() || query.is_empty() {
        println!("Usage: ./dbms --file <file> --query '<COMMAND> <name> [args]'");
        process::exit(1);
    }

    database::execute_query(&file_name, &query);
}
