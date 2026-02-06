#!/usr/bin/env python3
"""
Validate MOBKP instance files.

This script validates all instances in the instances folder, checking for:
- Duplicate nondominated points
- File format correctness

If duplicates are found, the script can optionally rewrite the instance
files without the repeated points.
"""

import argparse
from pathlib import Path
from typing import List, Set, Tuple


def parse_instance(file_path: Path) -> Tuple[int, int, int, List[Tuple[int, ...]], List[Tuple[int, ...]], int]:
    """
    Parse an instance file and return its components.
    
    Args:
        file_path: Path to the instance file
        
    Returns:
        Tuple of (n, m, W, items, non_points_list, claimed_size)
        
    Raises:
        ValueError: If the file format is invalid
    """
    with open(file_path, "r") as f:
        lines = f.readlines()
    
    if len(lines) < 3:
        raise ValueError(f"File too short: {file_path}")
    
    n, m = map(int, lines[0].strip().split())
    W = int(lines[1].strip())
    items = [tuple(map(int, line.strip().split())) for line in lines[2:2+n]]
    non_points_size = int(lines[2+n].strip())
    # Return as list to preserve duplicates for detection
    non_points_list = [tuple(map(int, line.strip().split())) for line in lines[3+n:]]
    
    return n, m, W, items, non_points_list, non_points_size


def find_duplicates(points: List[Tuple[int, ...]]) -> List[Tuple[int, ...]]:
    """
    Find duplicate points in a list.
    
    Args:
        points: List of points to check
        
    Returns:
        List of points that appear more than once
    """
    seen = set()
    duplicates = []
    for point in points:
        if point in seen and point not in duplicates:
            duplicates.append(point)
        seen.add(point)
    return duplicates


def write_instance(file_path: Path, n: int, m: int, W: int, 
                   items: List[Tuple[int, ...]], non_points: List[Tuple[int, ...]]) -> None:
    """
    Write an instance file without duplicate nondominated points.
    
    Args:
        file_path: Path to write the instance file
        n: Number of items
        m: Number of objectives
        W: Knapsack capacity
        items: List of item tuples (values + weight)
        non_points: List of nondominated points (duplicates will be removed)
    """
    unique_points = list(dict.fromkeys(non_points))  # Preserve order, remove duplicates
    with open(file_path, "w") as f:
        f.write(f"{n} {m}\n")
        f.write(f"{W}\n")
        for item in items:
            f.write(" ".join(map(str, item)) + "\n")
        f.write(f"{len(unique_points)}\n")
        for point in unique_points:
            f.write(" ".join(map(str, point)) + "\n")


def validate_instances(instances_folder: Path, modify: bool, verbose: bool) -> dict:
    """
    Validate all instances in the given folder.
    
    Args:
        instances_folder: Path to the instances directory
        modify: If True, rewrite files without duplicates
        verbose: If True, print detailed progress
        
    Returns:
        Dictionary with validation statistics
    """
    stats = {
        "total_files": 0,
        "files_with_duplicates": 0,
        "files_fixed": 0,
        "errors": 0
    }
    
    # Find all instance type folders
    instance_types = [d for d in instances_folder.iterdir() if d.is_dir()]
    
    if verbose:
        print(f"Found instance types: {[d.name for d in instance_types]}")
    
    for instance_type in instance_types:
        # Find all objectives folders
        objectives_folders = [d for d in instance_type.iterdir() if d.is_dir()]
        
        if verbose:
            print(f"\nProcessing '{instance_type.name}' with folders: {[d.name for d in objectives_folders]}")
        
        for objectives_folder in objectives_folders:
            # Find all .in files
            instances = list(objectives_folder.glob("*.in"))
            
            for instance_path in instances:
                stats["total_files"] += 1
                
                try:
                    n, m, W, items, non_points_list, claimed_size = parse_instance(instance_path)
                    unique_count = len(set(non_points_list))
                    
                    if unique_count != claimed_size:
                        duplicates_count = claimed_size - unique_count
                        stats["files_with_duplicates"] += 1
                        print(f"Error: {instance_path} has {duplicates_count} duplicate nondominated points")
                        
                        # Find and print first 3 duplicates
                        duplicates = find_duplicates(non_points_list)
                        for i, dup in enumerate(duplicates[:3]):
                            print(f"  Duplicate {i+1}: {dup}")
                        if len(duplicates) > 3:
                            print(f"  ... and {len(duplicates) - 3} more")
                        
                        if modify:
                            write_instance(instance_path, n, m, W, items, non_points_list)
                            stats["files_fixed"] += 1
                            print(f"  -> Fixed: removed duplicates")
                            
                except Exception as e:
                    stats["errors"] += 1
                    print(f"Error processing {instance_path}: {e}")
            
            if verbose:
                print(f"  Validated {len(instances)} instances in {objectives_folder.name}")
    
    return stats


def main() -> None:
    """Main entry point for the validation script."""
    parser = argparse.ArgumentParser(
        description="Validate MOBKP instance files for duplicates and format errors.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python validate.py                    # Dry run, report issues only
  python validate.py --modify           # Fix duplicate points in place
  python validate.py --verbose          # Show detailed progress
  python validate.py -d /path/to/inst   # Use custom instances folder
        """
    )
    parser.add_argument(
        "-d", "--directory",
        type=Path,
        default=Path("instances"),
        help="Path to instances directory (default: instances)"
    )
    parser.add_argument(
        "-m", "--modify",
        action="store_true",
        help="Modify files to remove duplicate nondominated points"
    )
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Print detailed progress information"
    )
    
    args = parser.parse_args()
    
    if not args.directory.exists():
        print(f"Error: Directory not found: {args.directory}")
        exit(1)
    
    print(f"Validating instances in: {args.directory}")
    print(f"Mode: {'modify' if args.modify else 'dry-run (report only)'}")
    print()
    
    stats = validate_instances(args.directory, args.modify, args.verbose)
    
    # Print summary
    print()
    print("=" * 50)
    print("Validation Summary")
    print("=" * 50)
    print(f"Total files checked:     {stats['total_files']}")
    print(f"Files with duplicates:   {stats['files_with_duplicates']}")
    if args.modify:
        print(f"Files fixed:             {stats['files_fixed']}")
    print(f"Errors encountered:      {stats['errors']}")
    
    if stats['files_with_duplicates'] == 0 and stats['errors'] == 0:
        print("\n✓ All instances validated successfully!")
    elif not args.modify and stats['files_with_duplicates'] > 0:
        print("\nRun with --modify to fix duplicate issues.")


if __name__ == "__main__":
    main()
