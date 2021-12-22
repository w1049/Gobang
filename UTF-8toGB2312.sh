for file in `find . -type f -regex ".*\.cpp\|.*\.h"`; do
   mv "$file" "$file.old" && iconv -f UTF-8 -t GB2312 < "$file.old" > "$file" && rm -rf "$file.old"
done