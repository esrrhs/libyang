package main

import (
	"flag"
	"fmt"
	"os"
)

func main() {
	xmlPath := flag.String("xml", "", "input message definition XML")
	outPath := flag.String("out", "", "output C++ header path")
	showVersion := flag.Bool("version", false, "show version")
	flag.Usage = func() {
		fmt.Fprintf(flag.CommandLine.Output(), "Usage: %s [flags] [<xml> <out.h>]\n\n", os.Args[0])
		fmt.Fprintf(flag.CommandLine.Output(), "Generate C++ structs and Marshal/Unmarshal from an XML message schema.\n\n")
		fmt.Fprintf(flag.CommandLine.Output(), "Flags:\n")
		flag.PrintDefaults()
		fmt.Fprintf(flag.CommandLine.Output(), "\nExamples:\n")
		fmt.Fprintf(flag.CommandLine.Output(), "  %s -xml sample.xml -out sample.h\n", os.Args[0])
		fmt.Fprintf(flag.CommandLine.Output(), "  %s sample.xml sample.h\n", os.Args[0])
	}
	flag.Parse()

	if *showVersion {
		fmt.Println(Version)
		return
	}

	xmlFile := *xmlPath
	outFile := *outPath
	args := flag.Args()
	if xmlFile == "" && outFile == "" && len(args) >= 2 {
		xmlFile = args[0]
		outFile = args[1]
	}
	if xmlFile == "" || outFile == "" {
		flag.Usage()
		os.Exit(2)
	}

	if err := GenerateFile(xmlFile, outFile); err != nil {
		fmt.Fprintln(os.Stderr, "error:", err)
		os.Exit(1)
	}
	fmt.Println("gen ok:", outFile)
}
