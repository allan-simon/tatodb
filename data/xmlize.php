<?php

ini_set('memory_limit','-1');
$sentence_exists = array();
$redirect = array();

function parse_sentences($path) {
	global $sentence_exists, $redirect;

	$handle = fopen($path, "r");
	if (!$handle)
		return;

	$sentence_str_exists = array();
	while ($line = fgets($handle, 4096)) {
		$cols = explode("\t", rtrim($line));
		if (count($cols) == 3) {
			list($id, $lang, $str) = $cols;
			$id = (int) $id;
			if (!isset($sentence_str_exists["$lang:$str"])) {
				$sentence_str_exists["$lang:$str"] = $id;
				$str = htmlspecialchars($str);
				echo "<item id=\"$id\" lang=\"$lang\" str=\"$str\"/>\n";
				$sentence_exists[$id] = true;
			}
			else {
				$redirect[$id] = $sentence_str_exists["$lang:$str"];
			}
		}
	}
	fclose($handle);
}

function parse_links($path) {
	global $sentence_exists, $redirect;
	$rel_exists = array();

	$handle = fopen($path, "r");
	if (!$handle)
		return;
	$id = 0;
	while ($line = fgets($handle, 1024)) {
		$cols = explode("\t", rtrim($line));
		if (count($cols) == 2) {
			list($x, $y) = $cols;

			$x = (int) $x;
			if (isset($redirect[$x]))
				$x = $redirect[$x];

			$y = (int) $y;
			if (isset($redirect[$y]))
				$y = $redirect[$y];

			if ($x < $y and !isset($rel_exists["$x-$y"]) and isset($sentence_exists[$x]) and isset($sentence_exists[$y])) {
				$rel_exists["$x-$y"] = true;
				$id++;
				echo "<rel id=\"$id\" x=\"$x\" y=\"$y\"/>\n";
			}
		}
	}
	fclose($handle);
}

echo "<tatodb>\n";
parse_sentences("sentences.csv");
parse_links("links.csv");
echo "</tatodb>";

?>
