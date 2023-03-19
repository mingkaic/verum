load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def gtest_repository(name):
    TAG = "v1.13.0"
    print("downloading gtest {} from verum".format(TAG))
    git_repository(
        name = name,
        remote = "https://github.com/google/googletest",
        tag = TAG,
    )
