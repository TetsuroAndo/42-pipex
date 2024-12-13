#!/usr/bin/env bash

# Cleanup old test artifacts
rm -f infile outfile outfile_expected outfile2 outfile2_expected

# 事前準備
echo "hello world" > infile
echo "foo bar baz" >> infile
echo "a b c" >> infile

# 成功・失敗カウンタ
TEST_SUCCESS=0
TEST_FAIL=0

function run_test () {
    # 引数:
    # 1: コマンドライン (pipex実行)
    # 2: シェル等価コマンド
    # 3: テスト名

    echo "-------------------------------------"
    echo "TEST: $3"
    echo "CMD: $1"
    echo "SHELL: $2"
    rm -f outfile outfile_expected
    eval "$1"
    eval "$2"
    if diff outfile outfile_expected > /dev/null; then
        echo "=> OK"
        TEST_SUCCESS=$((TEST_SUCCESS+1))
    else
        echo "=> NG"
        TEST_FAIL=$((TEST_FAIL+1))
        echo "Diff result:"
        diff outfile outfile_expected
    fi
}

##########################
# Mandatory Tests
##########################

# テスト1: 単純な2コマンドパイプ (必須部分)
# < infile ls -l | wc -l > outfile_expected
# ./pipex infile "ls -l" "wc -l" outfile
touch dummy_file
ls -l dummy_file > /dev/null 2>&1 # 確実に`ls -l`が動く環境前提
run_test \
"./pipex infile \"cat\" \"wc -l\" outfile" \
"< infile cat | wc -l > outfile_expected" \
"Mandatory: cat|wc -l"

# テスト2: コマンドが存在しない場合
run_test \
"./pipex infile \"not_a_command\" \"wc -l\" outfile" \
"< infile not_a_command | wc -l > outfile_expected 2>/dev/null" \
"Mandatory: non-existent command"
# ここはnon-existent commandのため比較が難しいが、not_a_commandはシェルで失敗するため、
# outfile_expectedは空になるはず。pipex側がエラーとなりoutfileが生成されなければOKとする。

##########################
# Bonus Tests: Multiple Pipes
##########################

# テスト3: 複数パイプ
# < infile cat | grep foo | wc -l > outfile_expected
run_test \
"./pipex infile \"cat\" \"grep foo\" \"wc -l\" outfile" \
"< infile cat | grep foo | wc -l > outfile_expected" \
"Bonus: multiple pipes (cat|grep foo|wc -l)"

# テスト4: 複数パイプ＋フィルタ
# < infile tr a-z A-Z | grep FOO | wc -w > outfile_expected
run_test \
"./pipex infile \"tr a-z A-Z\" \"grep FOO\" \"wc -w\" outfile" \
"< infile tr a-z A-Z | grep FOO | wc -w > outfile_expected" \
"Bonus: multiple pipes (tr|grep|wc)"

##########################
# Bonus Tests: here_doc
##########################

# テスト用にhere_doc入力をエコーバックするケース
# here_doc: ./pipex here_doc END "cat" "wc -l" outfile
# シェル等価: (echo "line1"; echo "line2") | wc -l > outfile_expected
# pipex側: echo "line1\nline2\nEND"を標準入力に流す必要があるのでecho -e使用
echo "line1
line2
END" | ./pipex here_doc END "cat" "wc -l" outfile
echo "line1
line2" | wc -l > outfile_expected

if diff outfile outfile_expected > /dev/null; then
    echo "-------------------------------------"
    echo "TEST: here_doc simple test"
    echo "CMD: (echo with END) | ./pipex here_doc END \"cat\" \"wc -l\" outfile"
    echo "=> OK"
    TEST_SUCCESS=$((TEST_SUCCESS+1))
else
    echo "-------------------------------------"
    echo "TEST: here_doc simple test"
    echo "=> NG"
    echo "outfile:"
    cat outfile
    echo "outfile_expected:"
    cat outfile_expected
    TEST_FAIL=$((TEST_FAIL+1))
    echo "Diff result:"
    diff outfile outfile_expected
fi

# テスト: here_doc + multiple commands
# コマンド: ./pipex here_doc STOP "tr a-z A-Z" "grep HELLO" "wc -c" outfile
# シェル等価: (echo "hello world"; echo "this is a test") | tr a-z A-Z | grep HELLO | wc -c > outfile_expected
echo "hello world
this is a test
STOP" | ./pipex here_doc STOP "tr a-z A-Z" "grep HELLO" "wc -c" outfile
echo "hello world
this is a test" | tr a-z A-Z | grep HELLO | wc -c > outfile_expected

if diff outfile outfile_expected > /dev/null; then
    echo "-------------------------------------"
    echo "TEST: here_doc multiple commands"
    echo "CMD: (echo lines with STOP) | ./pipex here_doc STOP \"tr a-z A-Z\" \"grep HELLO\" \"wc -c\" outfile"
    echo "=> OK"
    TEST_SUCCESS=$((TEST_SUCCESS+1))
else
    echo "-------------------------------------"
    echo "TEST: here_doc multiple commands"
    echo "=> NG"
    echo "outfile:"
    cat outfile
    echo "outfile_expected:"
    cat outfile_expected
    TEST_FAIL=$((TEST_FAIL+1))
    echo "Diff result:"
    diff outfile outfile_expected
fi

# テスト5: 環境変数PATHを使用するコマンド
run_test \
"./pipex infile \"which ls\" \"wc -l\" outfile" \
"< infile which ls | wc -l > outfile_expected" \
"Path resolution test: which ls|wc -l"

# テスト6: 複数の引数を持つコマンド
run_test \
"./pipex infile \"grep -i HELLO\" \"sed 's/world/everyone/'\" outfile" \
"< infile grep -i HELLO | sed 's/world/everyone/' > outfile_expected" \
"Multiple arguments test: grep -i|sed substitution"

# テスト7: 特殊文字を含むコマンド
run_test \
"./pipex infile \"tr ' ' '\t'\" \"tr '\t' ','\" outfile" \
"< infile tr ' ' '\t' | tr '\t' ',' > outfile_expected" \
"Special characters test: tr with tabs"

# テスト8: 長いパイプチェーン
run_test \
"./pipex infile \"cat\" \"grep a\" \"tr a-z A-Z\" \"wc -w\" outfile" \
"< infile cat | grep a | tr a-z A-Z | wc -w > outfile_expected" \
"Long pipe chain test: cat|grep|tr|wc"

##########################
# 結果まとめ
##########################
echo "====================================="
echo "Test results:"
echo "Success: $TEST_SUCCESS"
echo "Fail:    $TEST_FAIL"
echo "====================================="

rm -f infile outfile outfile_expected dummy_file
