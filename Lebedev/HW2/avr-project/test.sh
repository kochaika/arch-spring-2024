make build
make run-limited > actual-test-result.txt
diff actual-test-result.txt expected-test-result.txt
res=$?
if [ $res -eq 0 ]; then
  echo "\033[32mtest passed\033[0m"
else
  echo "\033[31mtest failed\033[0m"
fi
rm -f actual-test-result.txt
