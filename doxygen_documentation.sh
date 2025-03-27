#!/bin/bash
sudo apt install doxygen -y
doxygen -g

# Generate default Doxyfile if not present
if [ ! -f Doxyfile ]; then
    doxygen -g Doxyfile
fi

# Update settings using sed
sed -i 's/^PROJECT_NAME.*/PROJECT_NAME           = "graphlib"/' Doxyfile
sed -i 's/^PROJECT_BRIEF.*/PROJECT_BRIEF          = "A high-performance, secure C++ graph library using adjacency lists"/' Doxyfile
sed -i 's/^PROJECT_LOGO.*/PROJECT_LOGO           =/' Doxyfile

sed -i 's|^INPUT .*|INPUT                  = ./src ./include ./README.md|' Doxyfile
sed -i 's|^FILE_PATTERNS .*|FILE_PATTERNS          = *.hpp *.cpp *.md|' Doxyfile

sed -i 's|^OUTPUT_DIRECTORY .*|OUTPUT_DIRECTORY       = ./docs|' Doxyfile
sed -i 's/^GENERATE_HTML.*/GENERATE_HTML          = YES/' Doxyfile
sed -i 's/^HTML_OUTPUT.*/HTML_OUTPUT            = html/' Doxyfile
sed -i 's/^HTML_TIMESTAMP.*/HTML_TIMESTAMP         = YES/' Doxyfile

# If USE_MDFILE_AS_MAINPAGE is not in the file, add it
grep -q '^USE_MDFILE_AS_MAINPAGE' Doxyfile \
    && sed -i 's|^USE_MDFILE_AS_MAINPAGE.*|USE_MDFILE_AS_MAINPAGE = README.md|' Doxyfile \
    || echo 'USE_MDFILE_AS_MAINPAGE = README.md' >> Doxyfile

sed -i 's|^EXTENSION_MAPPING .*|EXTENSION_MAPPING      = md=markdown|' Doxyfile
sed -i 's|^MARKDOWN_SUPPORT .*|MARKDOWN_SUPPORT       = YES|' Doxyfile
sed -i 's|^GENERATE_TREEVIEW .*|GENERATE_TREEVIEW      = YES|' Doxyfile

echo "✅ Doxyfile configured successfully."

doxygen Doxyfile
