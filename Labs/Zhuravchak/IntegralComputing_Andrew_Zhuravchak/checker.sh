#!/bin/bash

# Init default values
PATH_WITH_PROGRAM='./cmake-build-release/ParallelIntegralComputing'
ITERATIONS=10
THREADS=1
ABSOLUTE=0.01
RELATIVE=0.01
XMIN=-50
XMAX=50
YMIN=-50
YMAX=50

# It will store the minimum computation time in all iterations
MIN_TIME=99999999999999

# Parse arguments
for i in "$@"
do
case $i in
    -p=*|--path=*)
    PATH_WITH_PROGRAM="${i#*=}"
    shift
    ;;
    -i=*|--iterations=*)
    ITERATIONS="${i#*=}"
    shift
    ;;
    -t=*|--threads=*)
    THREADS="${i#*=}"
    shift
    ;;
    -a=*|--absolute=*)
    ABSOLUTE="${i#*=}"
    shift
    ;;
    -r=*|--relative=*)
    RELATIVE="${i#*=}"
    shift
    ;;
    --xmin=*)
    XMIN="${i#*=}"
    shift
    ;;
    --xmax=*)
    XMAX="${i#*=}"
    shift
    ;;
    --ymin=*)
    YMIN="${i#*=}"
    shift
    ;;
    --ymax=*)
    YMAX="${i#*=}"
    shift
    ;;
    *)
          # unknown option
    ;;
esac
done


results=()

for (( i=1; i<= ${ITERATIONS}; i++ ))
do
   RESULT=$(${PATH_WITH_PROGRAM} -t ${THREADS} -a ${ABSOLUTE} -r ${RELATIVE} -x ${XMIN}:${XMAX} -y ${YMIN}:${YMAX})

   RES_ARR=( $RESULT )
   results[i]=${RES_ARR[3]}

   echo "Res"${i}": "${RES_ARR[3]} # Result

    # Check the time
    if ((${MIN_TIME} > ${RES_ARR[0]})); then
       MIN_TIME=${RES_ARR[0]}
    fi
done

CORRECT=1
# Check error in results array
for (( i=1; i<= ${ITERATIONS}; i++ ))
do
    this=${results[i]}
    if [ "$i" -eq "$ITERATIONS" ]; then
        next=${results[1]}
    else
        next=${results[i + 1]}
    fi

    REL=$(echo "1 - ($this / $next)" | bc)
    ABS=$(echo "$this - $next" | bc)

    REL_BOOL=$(echo "($REL < $RELATIVE)" | bc)
    ABS_BOOL=$(echo "($ABS < $ABSOLUTE)" | bc)

    if ((${REL_BOOL} == 1)) && ((${ABS_BOOL} == 1)); then
       continue
    else
        CORRECT=0
        echo "NOT CORRECT RESULT!!!"
        break
    fi
done

echo ""
if ((${CORRECT} == 1)) ; then
       echo "Everything is correct!!!"
fi

echo "Minimum computation time: "${MIN_TIME}