
mkdir -p ../lanarts_build
cd ../lanarts_build

cmake ../lanarts

## START TIMER CODE
T="$(date +%s%N)"
## START TIMER CODE

##MAKE
failed=0
cores=$(grep -c ^processor /proc/cpuinfo)
if ! make -j$((cores+1)) ; then
	failed=1
fi

## END TIMER CODE
T="$(($(date +%s%N)-T))"
echo "Total build time: $((T/1000000))ms"
## END TIMER CODE

exit "$failed"
