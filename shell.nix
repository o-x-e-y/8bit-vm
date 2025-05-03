{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    gcc
    valgrind
    clang-tools
  ];
}