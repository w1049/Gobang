for file in `find . -type f -regex ".*\.cpp\|.*\.h"`; do
   clang-format -i "$file"
done