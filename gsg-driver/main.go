package main

import "fmt"

func Run() error {
	return fmt.Errorf("Run() is not yet implemented.")
}

func main() {
	err := Run();
	if err != nil {
		fmt.Println("Got error : ", err);
	}
}
