{
  description = "SRPN Calculator.";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = inputs@{ self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        overlays = [ ];

        pkgs = import nixpkgs {inherit system overlays;};

        additionalPkgs = with pkgs; [ clang-tools gdb valgrind lua astyle zsh rnix-lsp];

        buildInputs = with pkgs; [ gnumake clang ];

        project = pkgs.stdenv.mkDerivation {
          name = "srpn";
          src = self;
          inherit buildInputs;
          nativeBuildInputs = buildInputs ++ additionalPkgs;
          fixupPhase = ''cp -r ./bin $out'';
          installPhase = ''mkdir -p $out/bin'';
          shellHook = "zsh";
        };

      in {
        # Used by `nix build` & `nix run` (prod exe)
        defaultPackage = project;

        # Used by `nix develop` (dev shell)
        devShell = project;
      });
}
