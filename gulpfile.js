// -----------------------------------------------------------------------------
// File system builder
// -----------------------------------------------------------------------------

const gulp = require('gulp');
const through = require('through2');

const htmlmin = require('gulp-htmlmin');
const inline = require('gulp-inline');
const inlineImages = require('gulp-css-base64');
const favicon = require('gulp-base64-favicon');
const crass = require('gulp-crass');

const htmllint = require('gulp-htmllint');
const csslint = require('gulp-csslint');

const rename = require('gulp-rename');
const replace = require('gulp-replace');
const remover = require('gulp-remove-code');
const gzip = require('gulp-gzip');
const path = require('path');

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------

const htmlFolder = 'html/';
const configFolder = 'config/';
const dataFolder = 'data/';
const staticFolder = 'lib/static/';

// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

var toHeader = function(name, debug) {

    return through.obj(function (source, encoding, callback) {

        var parts = source.path.split(path.sep);
        var filename = parts[parts.length - 1];
        var safename = name || filename.split('.').join('_');

        // Generate output
        var output = '';
        output += '#define ' + safename + '_len ' + source.contents.length + '\n';
        output += 'const uint8_t ' + safename + '[] PROGMEM = {';
        for (var i=0; i<source.contents.length; i++) {
            if (i > 0) { output += ','; }
            if (0 === (i % 20)) { output += '\n'; }
            output += '0x' + ('00' + source.contents[i].toString(16)).slice(-2);
        }
        output += '\n};';

        // clone the contents
        var destination = source.clone();
        destination.path = source.path + '.h';
        destination.contents = Buffer.from(output);

        if (debug) {
            console.info('Image ' + filename + ' \tsize: ' + source.contents.length + ' bytes');
        }

        callback(null, destination);

    });

};

var htmllintReporter = function(filepath, issues) {
    if (issues.length > 0) {
        issues.forEach(function (issue) {
            console.info(
                '[gulp-htmllint] ' +
                filepath + ' [' +
                issue.line + ',' +
                issue.column + ']: ' +
                '(' + issue.code + ') ' +
                issue.msg
            );
        });
        process.exitCode = 1;
    }
};

var buildWebUI = function(module) {

    return gulp.src(htmlFolder + module + '.html').
        pipe(htmllint({
            'failOnError': false,
            'rules': {
                'id-class-style': false,
                'label-req-for': false,
                //'line-end-style': false,
            }
        }, htmllintReporter)).
        pipe(favicon()).
        pipe(inline({
            base: htmlFolder,
            js: [],
            css: [crass, inlineImages],
            disabledTypes: ['svg', 'img']
        })).
        pipe(htmlmin({
            collapseWhitespace: true,
            removeComments: true,
            minifyCSS: true,
            minifyJS: true
        })).
        pipe(replace('pure-', 'p-')).
        pipe(gzip()).
        pipe(rename(module + '.html.gz')).
        pipe(gulp.dest(dataFolder)).
        pipe(toHeader(module+'_image', true)).
        pipe(gulp.dest(staticFolder));

};

// -----------------------------------------------------------------------------
// Tasks
// -----------------------------------------------------------------------------
gulp.task('index', function() {
    return buildWebUI('index');
});

gulp.task('upload', function() {
    return buildWebUI('upload');
});

gulp.task('webui',
    gulp.parallel(
        'index',
        'upload'
    )
);

gulp.task('default', gulp.series('webui'));

