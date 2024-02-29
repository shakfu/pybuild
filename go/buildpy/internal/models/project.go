package models

import (
	// "log"
	"os"
	"path/filepath"

	"github.com/shakfu/buildpy/internal/shell"
	"github.com/charmbracelet/log"
)

type Project struct {
	Cwd       string
	Build     string
	Downloads string
	Src       string
	Install   string
}

func NewProject() *Project {
	cwd, err := os.Getwd()
	if err != nil {
		log.Fatal(err)
	}
	var build = filepath.Join(cwd, "build")
	var downloads = filepath.Join(build, "downloads")
	var src = filepath.Join(build, "src")
	var install = filepath.Join(build, "install")
	return &Project{cwd, build, downloads, src, install}
}

func (p *Project) Setup() {
	shell.Makedirs(p.Build, p.Downloads, p.Src, p.Install)
}
