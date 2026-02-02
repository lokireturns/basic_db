package main

import (
	"bufio"
	"fmt"
	"io"
	"os/exec"
)

func initDbShell() (*exec.Cmd, io.WriteCloser, io.ReadCloser) {
	cmd := exec.Command("../db")
	stdin, err := cmd.StdinPipe()
	if err != nil {
		fmt.Println("Could not create stdin pipe:", err.Error())
	}
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		fmt.Println("Could not create stdout pipe:", err.Error())
	}
	// Use Start() to keep REPL process running
	if err := cmd.Start(); err != nil {
		fmt.Println("error starting command:", err.Error())
	}
	return cmd, stdin, stdout
}

func runCommand(commands []string, cmd *exec.Cmd, stdin io.WriteCloser, stdout io.ReadCloser) string {

	// Loop that runs each command and captures output of each
	println("Length of commands is ", len(commands))
	for j := 0; j < len(commands); j++ {
		println(commands[j])
		io.WriteString(stdin, commands[0]+"\n")

		scanner := bufio.NewScanner(stdout)
		scanner.Scan()
		scanner.Scan()
		fmt.Println(scanner.Text())

	}
	stdin.Close()
	cmd.Wait() // Reap the child process
	return ""
}

func main() {
	cmd, stdin, stdout := initDbShell()
	runCommand([]string{"select *"}, cmd, stdin, stdout)
}
