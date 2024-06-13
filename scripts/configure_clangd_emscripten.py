import subprocess
import json
import sys


build_dir = sys.argv[1]
print(f"build directory: {build_dir}")

compile_commands_path = f"{build_dir}/compile_commands.json"
print(f"compilation database: {compile_commands_path}")

compile_commands_file = open(compile_commands_path, "r")
compile_commands = json.load(compile_commands_file)
compile_commands_file.close()

compiler_path = compile_commands[0]["command"].split(" ")[0]
print(f"emscripten compiler: {compiler_path}")

cflags_result = subprocess.run([compiler_path, "--cflags"], stdout=subprocess.PIPE, text=True)
cflags = cflags_result.stdout.replace("\n", " ").replace("\\", "/").strip().split(" ")
print(f"compile flags: {cflags}")

config_file = open(".clangd", "w")
config_file.write("CompileFlags:\n")
config_file.write("  Add: \n")

for flag in cflags:
    config_file.write(f"    - \"{flag}\"\n")

config_file.write("  Remove: [\"-s*\"]\n")
config_file.close()

print("clangd configuration generated")