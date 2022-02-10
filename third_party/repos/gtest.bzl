load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def gtest_repository(name):
    git_repository(
        name = name,
        remote = "https://github.com/google/googletest",
        commit = "3e0e32ba300ce8afe695ad3ba7e81b21b7cf237a",
    )
