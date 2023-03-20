load("//third_party:repos/gtest.bzl", "gtest_repository")
load("//third_party:repos/cisab.bzl", "cisab_repository")
load("//third_party:repos/grpc.bzl", "grpc_repository")

def validate(dep, excludes, force_includes):
    return dep not in excludes or dep in force_includes

def dependencies(excludes = [], force_includes = []):
    ignores = native.existing_rules().keys() + excludes
    print("downloading verum dependencies. ignoring {}".format(ignores))

    if validate("com_github_mingkaic_cisab", ignores, force_includes):
        cisab_repository()

    if validate("gtest", ignores, force_includes):
        gtest_repository(name = "gtest")

    if validate("com_github_grpc_grpc", ignores, force_includes):
        grpc_repository()
