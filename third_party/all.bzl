load("//third_party:repos/gtest.bzl", "gtest_repository")
load("//third_party:repos/cisab.bzl", "cisab_repository")
load("//third_party:repos/grpc.bzl", "grpc_repository")

def dependencies(excludes = []):
    ignores = native.existing_rules().keys() + excludes

    if "com_github_mingkaic_cisab" not in ignores:
        cisab_repository()

    if "gtest" not in ignores:
        gtest_repository(name = "gtest")

    if "com_github_grpc_grpc" not in ignores:
        grpc_repository()
