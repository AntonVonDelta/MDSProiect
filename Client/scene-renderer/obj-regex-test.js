const OBJ_LINE_REGEX = /^(v(\s+-?(0|[1-9]\d*)(\.\d+)?){3,4}|vn(\s+-?(0|[1-9]\d*)(\.\d+)?){3}|vt(\s+-?(0|[1-9]\d*)(\.\d+)?){1,3}|f(\s+[1-9]\d*((\/([1-9]\d*)?)?\/[1-9]\d*)?){3,})$/

const tests = [
    ['v 0.123 0.234 0.345 1.0', true],
    ['v 0.123 0.234 0.345', true],
    ['v 1 1 1', true],
    ['v 1 1 -1 0.23456', true],
    ['v -2.33 -4.6 789 -0.99', true],
    ['v -2.33 -4.6 789 --0.99', false],
    ['v -2.33 -4.6', false],
    ['', false],
    ['v ', false],
    ['v 1 1 -1  0.23456', true],
    ['v  1 1 -1 0.23456', true],

    ['vn 0.123 0.234 0.345 1.0', false],
    ['vn 0.123 0.234 0.345', true],
    ['vn 1 1 1', true],
    ['vn 1 1 -1 0.23456', false],
    ['vn -2.33 -4.6 789 -0.99', false],
    ['vn -2.33 -4.6 789 --0.99', false],
    ['vn -2.33 -4.6', false],
    ['vn ', false],
    ['vn 1 1 -1  0.23456', false],
    ['vn  1 1 -1 0.23456', false],
    ['abcd 0.5 0.5 0.5', false],
    ['vn 1', false],
    ['vn 1 1', false],
    ['vn 1 1 1 1', false],

    ['vt 0.123 0.234 0.345 1.0', false],
    ['vt 0.123 0.234 0.345', true],
    ['vt 1 1 1', true],
    ['vt 1 1 -1 0.23456', false],
    ['vt -2.33 -4.6 789 -0.99', false],
    ['vt -2.33 -4.6 789 --0.99', false],
    ['vt -2.33 -4.6', true],
    ['vt ', false],
    ['vt 1 1 -1  0.23456', false],
    ['vt  1 1 -1 0.23456', false],
    ['abcd 0.5 0.5 0.5', false],
    ['vt 1', true],
    ['vt 1 1', true],
    ['vt 1 1 1 1', false],

    ['f', false],
    ['f 1', false],
    ['f 1 1', false],
    ['f 1 1 1', true],
    ['f 1 1 1 1', true],
    ['f 1 1 1 1 1', true],
    ['f 1 1 1 1 1 1', true],
    ['f 1 -1 1', false],
    ['f 1 -1 1 1', false],
    ['f 1 -1 1 1 1', false],
    ['f 1 -1 1 1 1 1', false],
    ['f 0.2 1.1 0.5', false],
    ['f 0.2 1.1 0.5 1', false],
    ['f 0.2 1.1 0.5 1 1', false],
    ['f 0.2 1.1 0.5 1 1 1', false],
    ['f 2/ 2/ 2/', false],
    ['f 2/ 22/ 123/', false],
    ['f 89/1 2/ 5/', false],
    ['f 20/1 2//7 89/', false],
    ['f ', false],
    ['f  1', false],
    ['f  1 1', false],
    ['f  1 1 1', true],
    ['f  1 1 1 1', true],
    ['f  1 1 1 1 1', true],
    ['f  1 1 1 1 1 1', true],
    ['f 1 ', false],
    ['f 1  1', false],
    ['f 1  1 1', true],
    ['f 1  1 1 1', true],
    ['f 1  1 1 1 1', true],
    ['f 1  1 1 1 1 1', true],
    ['f 12223 4456 7789 999', true],
    ['f 12223 4456 0 999', false],
    ['f -12223 4456 7789 999', false],
    ['f 1/1 1/1 1/1', true],
    ['f 1/1 1/1 1/-1', false],
    ['f 1/1/22 1/1/33 1/1/44 5', true],
    ['f 1/1/22 1/1/33 1/-1/44 5', false],
    ['f 11//22 33//44 55//66', true],
    ['f 11//22 33//44 0//66', false],
    ['f 1/2/3 4 5/6/7 8/9/10', true],
    ['f 1/2/3 4 5/0/7 8/9/10', false],
    ['f 1/-2/3 4 5/6/7 8/9/10', false],
    ['f 1/1', false]
]

let numTestsFailed = 0
tests.forEach(([test, expected]) => {
    const actual = OBJ_LINE_REGEX.test(test)
    if (actual !== expected) {
        console.error(`Test failed: ${test}. Result: ${actual}. Expected: ${expected}.`)
        numTestsFailed++
    }
})
console.log(`Ran ${tests.length} test(s).`)
if (numTestsFailed > 0) {
    throw new Error(`${numTestsFailed} test(s) failed.`)
}
console.log(`${numTestsFailed} test(s) failed.`)
