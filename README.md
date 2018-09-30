# graalvm-native-demo

1. Download graalvm

# Install additional languages

# TODO: figure out why this isn't working
`graalvm/graalvm-ce-1.0.0-rc6/bin/gu install -c python`

# Rebuild polyglot api
`./graalvm/graalvm-ce-1.0.0-rc6/jre/lib/svm/bin/rebuild-images libpolyglot`

# Compile
`gcc -Igraalvm/graalvm-ce-1.0.0-rc6/jre/lib/polyglot -Lgraalvm/graalvm-ce-1.0.0-rc6/jre/lib/polyglot -lpolyglot -o extendc -Wl,-rpath=graalvm/graalvm-ce-1.0.0-rc6/jre/lib/polyglot main.c`
