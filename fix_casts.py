import re

# Read the file
with open('src/evaluation.cpp', 'r') as f:
    content = f.read()

# Pattern to match dynamic_cast followed by -> operator on the same line
pattern = r'(\s*)(\w+\*)\s+(\w+)\s*=\s*dynamic_cast<(\w+\*)>\(([^)]+)\);\s*\n\s*([^\n]*?)\3->'

def replace_cast(match):
    indent = match.group(1)
    ptr_type = match.group(2)
    var_name = match.group(3)
    cast_type = match.group(4)
    expr = match.group(5)
    usage = match.group(6)
    
    # Create the replacement with null check
    replacement = f"{indent}{ptr_type} {var_name} = dynamic_cast<{cast_type}>({expr});\n"
    replacement += f"{indent}if (!{var_name}) throw RuntimeError(\"Invalid value\");\n"
    replacement += f"{indent}{usage}{var_name}->"
    
    return replacement

# Apply the replacement
new_content = re.sub(pattern, replace_cast, content, flags=re.MULTILINE)

# Write back
with open('src/evaluation.cpp', 'w') as f:
    f.write(new_content)

print("Fixed dynamic_cast issues")
