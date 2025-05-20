{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    gcc
    valgrind
    clang-tools
    sdl3
    pkg-config
  ];

  shellHook = ''
    cat > .clangd << EOF
    CompileFlags:
      Add: [
        -I${pkgs.sdl3.dev}/include,
        -xc
      ]
    EOF
    
    echo "generated .clangd config"
  '';
}