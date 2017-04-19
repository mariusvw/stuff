<!doctype html>
<html>
    <head>
        <meta charset="utf-8">
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title>Devtools Responsive</title>
        <style>
        body {
            text-align: center;
        }
        iframe {
            border: 1px solid #000;
        }
        </style>
    </head>
    <body>
    <?php
    // Sizes: width, height
    $devices = [
        ['name' => 'iPhone 5 portrait', 'w' => 320, 'h' => 568, 'pt' => 20, 'pb' => 44],
        ['name' => 'iPhone 5 landscape', 'w' => 568, 'h' => 320, 'pt' => 20, 'pb' => 44],
        ['name' => 'iPhone 6 portrait', 'w' => 375, 'h' => 667, 'pt' => 20, 'pb' => 44],
        ['name' => 'iPhone 6 landscape', 'w' => 667, 'h' => 375, 'pt' => 20, 'pb' => 44],
        ['name' => 'iPhone 6 Plus portrait', 'w' => 414, 'h' => 736, 'pt' => 20, 'pb' => 44],
        ['name' => 'iPhone 6 Plus landscape', 'w' => 736, 'h' => 414, 'pt' => 20, 'pb' => 44],
        ['name' => 'iPad Mini portrait', 'w' => 768, 'h' => 1024, 'pt' => 20, 'pb' => 44],
        ['name' => 'iPad Mini landscape', 'w' => 1024, 'h' => 768, 'pt' => 20, 'pb' => 44],
        ['name' => 'iPad Pro portrait', 'w' => 1024, 'h' => 1366, 'pt' => 20, 'pb' => 44],
        ['name' => 'iPad Pro landscape', 'w' => 1366, 'h' => 1024, 'pt' => 20, 'pb' => 44]
    ];

    foreach ($devices as $d) {
        echo $d['name'] . '<br><iframe src="http://mariusvw.com" style="width: ' . $d['w'] . 'px; height: ' . ($d['h'] - $d['pt'] - $d['pb']) . 'px;"></iframe><br><br>';
    }
    ?>
    </body>
</html>
