package main

import (
	"io"
	"os/exec"
	"strings"
	"testing"
)

func runScript(commands []string) []string {
	cmd := exec.Command("../db")

	stdin, err := cmd.StdinPipe()
	if err != nil {
		panic(err)
	}

	stdout, err := cmd.StdoutPipe()
	if err != nil {
		panic(err)
	}

	if err := cmd.Start(); err != nil {
		panic(err)
	}

	for _, command := range commands {
		io.WriteString(stdin, command+"\n")
	}

	stdin.Close()

	output, err := io.ReadAll(stdout)
	if err != nil {
		panic(err)
	}

	cmd.Wait()

	lines := strings.Split(string(output), "\n")
	return lines
}

func TestInsertsAndRetrievesRow(t *testing.T) {
	result := runScript([]string{
		"insert 1 user1 person1@example.com",
		".exit",
	})

	expected := []string{
		"db > (1, user1, person1@example.com)",
		"Executed ",
		"db > ",
	}

	if len(result) != len(expected) {
		t.Errorf("Expected %d lines, got %d", len(expected), len(result))
	}

	for i, line := range expected {
		if i < len(result) && result[i] != line {
			t.Errorf("Line %d: expected '%s', got '%s'", i, line, result[i])
		}
	}
}
