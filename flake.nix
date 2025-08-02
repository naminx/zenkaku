{
  description = "C++23 development shell & build for zenkaku";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";
  };

  outputs =
    {
      self,
      nixpkgs,
    }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        pname = "zenkaku";
        version = "1.0.0";
        src = ./.;

        nativeBuildInputs = with pkgs; [
          cmake
          clang
          pkg-config
        ];
        buildInputs = with pkgs; [
          cli11
        ];

        cmakeFlags = [
          "-DCMAKE_CXX_STANDARD=23"
        ];

        NIX_CFLAGS_COMPILE = ''
          -I${pkgs.cli11}/include
        '';
      };

      devShells.${system}.default = pkgs.mkShell {
        name = "zenkaku-shell";
        packages = with pkgs; [
          clang
          cmake
          clang-tools
          pkg-config
          cli11
        ];

        shellHook = ''
          export CC=clang
          export CXX=clang++
          export CXXFLAGS="-std=c++23"
          if [ -t 1 ] && command -v fish >/dev/null && [ -z "$IN_NIX_SHELL_FISH" ]; then
            export IN_NIX_SHELL_FISH=1
            exec fish
          fi
        '';

        NIX_CFLAGS_COMPILE = ''
          -I${pkgs.cli11}/include
        '';
      };
    };
}
