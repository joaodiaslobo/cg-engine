import re
import sys
import math
import numpy as np

def parse_rib_patches(filename):
    with open(filename, 'r') as f:
        content = f.read()

    lines = content.splitlines()
    patches = []
    transform_stack = [np.identity(4)]

    def current_transform():
        return transform_stack[-1]

    def apply_transform(pt, matrix):
        vec = np.array([pt[0], pt[1], pt[2], 1.0])
        transformed = matrix @ vec
        return tuple(transformed[:3])

    def parse_translate(args):
        x, y, z = map(float, args)
        mat = np.identity(4)
        mat[:3, 3] = [x, y, z]
        return mat

    def parse_scale(args):
        x, y, z = map(float, args)
        mat = np.identity(4)
        mat[0, 0] = x
        mat[1, 1] = y
        mat[2, 2] = z
        return mat

    def parse_rotate(args):
        angle, x, y, z = map(float, args)
        rad = math.radians(angle)
        axis = np.array([x, y, z])
        axis = axis / np.linalg.norm(axis)
        x, y, z = axis
        c = math.cos(rad)
        s = math.sin(rad)
        t = 1 - c
        rot = np.array([
            [t*x*x + c,   t*x*y - s*z, t*x*z + s*y, 0],
            [t*x*y + s*z, t*y*y + c,   t*y*z - s*x, 0],
            [t*x*z - s*y, t*y*z + s*x, t*z*z + c,   0],
            [0, 0, 0, 1]
        ])
        return rot

    control_points = []
    inside_transform = False

    i = 0
    while i < len(lines):
        line = lines[i].strip()
        if line.startswith("TransformBegin"):
            transform_stack.append(current_transform().copy())
            i += 1
            continue
        elif line.startswith("TransformEnd"):
            transform_stack.pop()
            i += 1
            continue
        elif line.startswith("Translate"):
            args = line.split()[1:]
            T = parse_translate(args)
            transform_stack[-1] = current_transform() @ T
        elif line.startswith("Scale"):
            args = line.split()[1:]
            S = parse_scale(args)
            transform_stack[-1] = current_transform() @ S
        elif line.startswith("Rotate"):
            args = line.split()[1:]
            R = parse_rotate(args)
            transform_stack[-1] = current_transform() @ R
        elif 'Patch "bicubic"' in line:
            match = re.search(r'\[(.*?)\]', line)
            if not match:
                raise ValueError(f"Malformed Patch line: {line}")
            nums = list(map(float, match.group(1).strip().split()))
            if len(nums) % 3 != 0:
                raise ValueError("Control point list not a multiple of 3.")
            pts = [(nums[j], nums[j+1], nums[j+2]) for j in range(0, len(nums), 3)]
            if len(pts) != 16:
                raise ValueError("Each bicubic patch must have 16 control points.")
            transformed_pts = [apply_transform(pt, current_transform()) for pt in pts]
            patches.append(transformed_pts)
            control_points.extend(transformed_pts)
        i += 1

    # Deduplicate control points
    unique_points = []
    point_to_index = {}

    for pt in control_points:
        rounded = tuple(round(x, 6) for x in pt)  # avoid floating point errors
        if rounded not in point_to_index:
            point_to_index[rounded] = len(unique_points)
            unique_points.append(rounded)

    patch_indices = []
    for patch in patches:
        indices = [point_to_index[tuple(round(x, 6) for x in pt)] for pt in patch]
        patch_indices.append(indices)

    return patch_indices, unique_points


def write_bezier_patch_format(output_filename, patch_indices, unique_points):
    with open(output_filename, 'w') as f:
        f.write(f"{len(patch_indices)}\n")
        for patch in patch_indices:
            f.write(', '.join(map(str, patch)) + "\n")
        f.write(f"{len(unique_points)}\n")
        for pt in unique_points:
            f.write(f"{pt[0]}, {pt[1]}, {pt[2]}\n")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python rib_to_patch.py <input.rib> <output.patch>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    patch_indices, unique_points = parse_rib_patches(input_file)
    write_bezier_patch_format(output_file, patch_indices, unique_points)
    print(f"Converted {len(patch_indices)} patches and {len(unique_points)} unique control points.")
