fn main() {
    let x = 10 + 20;           // ← CPU executes addition directly
    println!("result: {}", x);   // ← Needs system call (write to terminal)
}