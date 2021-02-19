#!/bin/bash
# VCGen, v0.0.2

CONTPATH=""
PASSLEN=25
VCEXEC="/usr/bin/veracrypt"
MAXITER=1
SIZE=4000000000
FSYS="FAT"
PIM=0
passPrefix=""
passPostfix=""

EXPORTPASSW=0
testVersion=1

encryption="AES"
hash="SHA-512"
secretsFile="./passwords.txt"
randomChars=320
randomFile="./random.txt"
filenameLen=15

CONTAINERS=()
PASSWORDS=()

print_out() {
	echo -n "$1" && tput sgr0;
	if [ -z "$2" ]; then echo; fi
}

print_error () {
	tput sgr0 && tput bold && tput setaf 1;
	print_out "$1" "$2"
}

print_ok () {
	tput sgr0 && tput bold && tput setaf 2;
	print_out "$1" "$2"
}

print_im () {
	tput sgr0 && tput bold && tput setaf 3;
	print_out "$1" "$2"
}

print_info () {
	tput sgr0 && tput bold && tput setaf 6;
	print_out "$1" "$2"
}

print_title () {
	tput sgr0 && tput bold && tput setaf 5 && tput setab 4 && echo;
	print_out "$1" "$2" 0
  	echo
}

printArgs()
{
	print_info "----------------------"
	# print_info "Args: "
	print_info "CONTPATH: $CONTPATH"
	print_info "PASSLEN: $PASSLEN"
	print_info "VCEXEC: $VCEXEC"
	print_info "MAXITER: $MAXITER"
	print_info "SIZE: $SIZE"
	print_info "FSYS: $FSYS"
	print_info "PIM: $PIM"
	print_info "EXPORTPASSW: $EXPORTPASSW"
	# echo
	# print_info "passPrefix: $passPrefix"
	# print_info "passPostfix: $passPostfix"
	print_info "----------------------"
}

checkArgs()
{
	argn=0
	while (( "$#" )); do

        arg=$(echo $1 | cut -d'=' -f1)
        parameter=$(echo $1 | cut -d'=' -f2)

        case "$arg" in
	
		"--ppref")
                	passPrefix="$parameter"
         	;;

		"--ppost")
                	passPostfix="$parameter"
         	;;

		"--vcexec")
                	VCEXEC="$parameter"
         	;;

		"--exp")
                	EXPORTPASSW=1
         	;;

		*)
			argn=$(( argn + 1 ))

			if [[ $argn -eq 1 ]]; then CONTPATH="$arg"; fi
			if [[ $argn -eq 2 ]]; then PASSLEN="$arg"; fi
			if [[ $argn -eq 3 ]]; then MAXITER="$arg"; fi
			if [[ $argn -eq 4 ]]; then SIZE="$arg"; fi
			if [[ $argn -eq 5 ]]; then FSYS="$arg"; fi
			if [[ $argn -eq 6 ]]; then PIM="$arg"; fi
                ;;

            esac

        shift
    done
}

runGenerator()
{
	prl=$(expr length "$passPrefix")
	pol=$(expr length "$passPostfix")
	prpol=$(( prl + pol ))

	i=0
	rv=0
	while [ "$rv" -eq 0 ]; do
		(( i++ ))

		if [[ $MAXITER -ne -1 ]] && [[ $i -gt $MAXITER ]]; then break; fi

		echo
		print_info "Iteration: $i"

		# echo "Generating $randomFile[$randomChars]"
		tr -dc '[:graph:]' </dev/urandom 2>/dev/null | tr -d \''\\'\` 2>/dev/null | head -c $randomChars >$randomFile

		genPassLen=$(( PASSLEN - prpol ))
		# echo "Generating password of length: $genPassLen ($PASSLEN - $prpol)"
		PASSWORD=$passPrefix
		genPass=$(openssl rand -base64 150 | tr -d '\n\r\t=+/')
		PASSWORD+=${genPass:0:((genPassLen))}
		PASSWORD+=$passPostfix

		CONTAINER=$(tr -dc "[:alnum:]" </dev/urandom 2>/dev/null | head -c $filenameLen)

		echo "Creating container:\"$CONTPATH/$CONTAINER\" of length:$SIZE B and fs:$FSYS..."
		echo -n "Start: " && date +%T
		$VCEXEC -t -c "$CONTPATH/$CONTAINER" --random-source $randomFile -k '' --volume-type=normal --size=$SIZE --pim $PIM --encryption=$encryption --password="$PASSWORD" --hash=$hash --filesystem=$FSYS # &>/dev/null
		rv=$?
		echo -n "Finish: " && date +%T

		if [ "$rv" -eq 0 ]; then
			print_im "$CONTAINER	" 0 && print_ok "OK"
		else
			print_im "$CONTAINER	" 0 && print_error "ERR"
		fi

		if [ "$rv" -eq 0 ]; then
			CONTAINERS+=("$CONTAINER")
			PASSWORDS+=("$PASSWORD")
		fi
		
		rm $randomFile
		echo
	done
}

print_information()
{
	echo
	print_im '------------------------------------------------------------------------'
	echo "Encryption: $encryption"
	echo "Hash: $hash"
	echo "Pim: $PIM"
	echo
	print_im "CONTAINER		PASSWORD"
	for (( i=0; i<${#CONTAINERS[@]}; i++ )) do
		print_im "${CONTAINERS[$i]}		${PASSWORDS[$i]}"
		if [[ $EXPORTPASSW -eq 1 ]]; then
			echo "${CONTAINERS[$i]}		${PASSWORDS[$i]}" >> $secretsFile
		fi
	done
	print_im '------------------------------------------------------------------------'
	echo
}

#----------------------------------------------------------------------

print_title "VCGen, v0.0.2"
checkArgs "$@"
if [[ -z "$CONTPATH" ]] || [[ "$CONTPATH" == "" ]]; then print_error "At least 1 argument is required."; exit 1; fi
printArgs

if [[ ! -f "$VCEXEC" ]]; then print_error "File \"$VCEXEC\" not found"; exit 2; fi

if [[ $testVersion -eq 1 ]]; then
	testVer=$( $VCEXEC -t --version | grep "VeraCrypt" )
	echo "$testVer"
	if [[ -z $testVer ]]; then print_error "Unexpected output of \"$VCEXEC\". Exiting..."; exit 2; fi
fi

runGenerator
print_information

echo FINISHED
exit 0

#----------------------------------------------------------------------
