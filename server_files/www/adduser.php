<?php
parse_str($_SERVER['QUERY_STRING'], $output);
?>
<html>
<body>
<?php
echo "<h2>Firstname: " . $output['firstname'] . "</h2></br>";
echo "<h2>Lastname: " . $output['lastname'] . "</h2></br>";
?>
</body>
</html>
