<DOCUMENT html>
    <head>
        <title>Projects</title>
    </head>
    <body>
        <?php foreach (glob('*', GLOB_ONLYDIR) as $project) { echo (preg_match('/^[a-z0-9\-]+$/', $project)) ? '<a href="' . $project . '">' . $project . '</a><br>' : ''; } ?>
    </body>
</html>
