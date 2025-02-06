use std::io::{self, Read, Write};
use std::net::{TcpStream, ToSocketAddrs};

fn main() -> io::Result<()> {
    // Parse command-line arguments
    let args: Vec<String> = std::env::args().collect();
    if args.len() != 5 || args[1] != "-h" || args[3] != "-p" {
        eprintln!("Usage: {} -h <host-ip> -p <port>", args[0]);
        std::process::exit(1);
    }
    let host = &args[2];
    let port: u16 = args[4].parse().expect("Invalid port number");

    // Connect to the database server
    let address = format!("{}:{}", host, port);
    let mut stream = TcpStream::connect(address.clone())?;
    println!("Connected to {} successfully!", address);

    // Read input from the user and send commands to the server
    let mut input = String::new();
    loop {
        print!("{} > ", address);
        io::stdout().flush()?;
        input.clear();
        io::stdin().read_line(&mut input)?;

        // Exit if user inputs 'exit' or presses Ctrl+d
        if input.trim() == "exit" || input.is_empty() {
            break;
        }

        print!("{}", input);
        // Send the command to the server
        stream.write_all(input.as_bytes())?;

        // Read the response from the server
        let mut buffer = [0; 1024];
        stream.read(&mut buffer)?;
        let response = String::from_utf8_lossy(&buffer);
        println!("{}", response);
    }

    Ok(())
}
