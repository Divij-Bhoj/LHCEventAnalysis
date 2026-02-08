#!/bin/bash
# Clean up unwanted files before Git upload

echo "=== Cleaning Project for GitHub Upload ==="
cd ~/cern-workspace/LHCEventAnalysis

echo ""
echo "1. Removing Zone.Identifier files..."
find . -name "*:Zone.Identifier" -type f -print -delete
echo "   ✓ Done"

echo ""
echo "2. Removing build artifacts..."
rm -rf build/
rm -f src/*.o include/*.o
rm -f lib/*.so lib/*.a
echo "   ✓ Done"

echo ""
echo "3. Removing data files (too large for Git)..."
rm -f data/*.root
rm -f results/*.root results/*.png results/*.pdf
echo "   ✓ Done"

echo ""
echo "4. Verifying .gitignore exists..."
if [ -f .gitignore ]; then
    echo "   ✓ .gitignore present"
else
    echo "   ✗ WARNING: .gitignore missing!"
fi

echo ""
echo "5. Verifying LICENSE exists..."
if [ -f LICENSE ]; then
    echo "   ✓ LICENSE present"
    # Check if it's customized
    if grep -q "\[Your Full Name\]" LICENSE; then
        echo "   ⚠  WARNING: LICENSE still has placeholder - customize it!"
    fi
else
    echo "   ✗ WARNING: LICENSE missing!"
fi

echo ""
echo "6. Files that will be uploaded:"
git init 2>/dev/null
git add . 2>/dev/null
git status --short | wc -l
echo "   files to upload"

echo ""
echo "7. Checking for unwanted files..."
UNWANTED=$(git ls-files | grep -E "\.root$|\.o$|build/|Zone\.Identifier" || true)
if [ -z "$UNWANTED" ]; then
    echo "   ✓ No unwanted files detected"
else
    echo "   ✗ WARNING: Found unwanted files:"
    echo "$UNWANTED"
fi

echo ""
echo "=== Cleanup Complete ==="
echo ""
echo "Next steps:"
echo "  1. Edit LICENSE and replace [Your Full Name]"
echo "  2. Edit README.md and replace YOUR_USERNAME"
echo "  3. Run: git add . && git commit -m 'Initial commit'"
echo "  4. Push to GitHub"
