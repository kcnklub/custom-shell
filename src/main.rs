use std::{
    io::{self, stdout, Write},
    process,
};

enum Builtin {
    EXIT,
    PROGRAM,
}

impl From<&str> for Builtin {
    fn from(value: &str) -> Self {
        match value {
            "exit" => Self::EXIT,
            _ => Self::PROGRAM,
        }
    }
}

fn main() -> Result<(), io::Error> {
    println!("Welcome to my custom shell");
    loop {
        stdout().write("> ".as_bytes())?;
        stdout().flush()?;
        let mut line = String::new();
        io::stdin().read_line(&mut line)?;
        let args: Vec<&str> = line.split(' ').map(|t| t.trim()).collect();
        let c: Builtin = args[0].into();
        match c {
            Builtin::EXIT => break,
            Builtin::PROGRAM => match execute_command(&args) {
                Ok(_) => {}
                Err(error) => eprintln!("Unable to run command successfully: {:?}", error),
            },
        }
    }

    println!("Goodbye!");
    Ok(())
}

fn execute_command(args: &[&str]) -> Result<(), std::io::Error> {
    let program = args[0];
    let args = args[1..].to_vec();
    let mut cmd = process::Command::new(program);
    cmd.args(args);
    let handle = cmd.spawn();
    match handle {
        Ok(mut c) => {
            c.wait()?;
            Ok(())
        }
        Err(error) => Err(error),
    }
}
