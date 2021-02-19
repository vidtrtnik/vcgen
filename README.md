# vcgen

## Introduction
<b>vcgen</b> is a bash shell script that generates multiple VeraCrypt containers.  
:warning: It is unfinished, untested and less secure than using the VeraCrypt directly. Use it at your own risk. :warning:


The script generates random filenames and passwords and runs VeraCrypt with the specified arguments. The hash function is set to SHA-512, the encryption is AES and the PIM value is set to 0. By default the container size is 4 GB and the filesystem is FAT. The randomly generated password contains 25 characters.
See the script for details. 

## Usage
Usage: <code class="language-plaintext highlighter-rouge">./vcgen.sh "/target/path/" [password length] [iterations] [container size in Bytes] [conatiner FS] [PIM] [--vcexec="/path/to/veracrypt"] [--ppref="pass prefix"] [--ppost="pass postfix"] [--exp]"</code>  

Example: <code class="language-plaintext highlighter-rouge">./vcgen.sh "./containers/" 30 2 1000000000 "FAT" 0 --vcexec="/usr/bin/veracrypt"</code>  

The <code class="language-plaintext highlighter-rouge">--vcexec</code> option sets the path to the VeraCrypt executable.  
The <code class="language-plaintext highlighter-rouge">--ppref</code> option specifies the password prefix.  
The <code class="language-plaintext highlighter-rouge">--ppost</code> option specifies the password postfix.  
The <code class="language-plaintext highlighter-rouge">--exp</code> option outputs the containers and their passwords to the file <code class="language-plaintext highlighter-rouge">passwords.txt</code> :warning: (in plaintext!).  

## vcgen-gui
<b>vcgen-gui</b> is a simple GUI program for vcgen. It is written in C using GTK toolkit. The GUI was designed using the Glade tool.  
The vcgen-gui is unfinished, untested and has a lot of missing features. The source code can be found in the folder <code class="language-plaintext highlighter-rouge">./vcgen-gui</code>.
