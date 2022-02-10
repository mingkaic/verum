workspace(name = "com_github_mingkaic_verum")

# === load local dependencies ===

load("//third_party:all.bzl", "dependencies")
dependencies()

# === development ===

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
git_repository(
    name = "com_grail_bazel_compdb",
	remote = "https://github.com/grailbio/bazel-compilation-database",
	tag = "0.4.5",
)
