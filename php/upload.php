<?php
// Code based on 
// Rui Santos
// Complete project details at https://RandomNerdTutorials.com/esp32-cam-post-image-photo-server/

$target_dir    = "uploads/";
$datum         = mktime(date('H')+0, date('i'), date('s'), date('m'), date('d'), date('y'));
$target_file   = $target_dir . date('Y.m.d_H:i:s_', $datum) . basename($_FILES["imageFile"]["name"]);
$uploadOk      = 1;
$imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));

// Check if file already exists
if (file_exists($target_file)) {
  echo "Sorry, file already exists.";
  $uploadOk = 0;
}

// Check file size
if ($_FILES["imageFile"]["size"] > 500000) {
  echo "Sorry, your file is too large.";
  $uploadOk = 0;
}

// Allow certain file formats
if ($imageFileType != "jpg" && $imageFileType != "png" && $imageFileType != "jpeg" && 
    $imageFileType != "gif" && $imageFileType != "txt") {
  echo "Sorry, only JPG, JPEG, PNG & GIF, TXT files are allowed.";
  $uploadOk = 0;
}

// Check if $uploadOk is set to 0 by an error
if ($uploadOk == 0) {
  echo "Sorry, your file was not uploaded.";
  // if everything is ok, try to upload file
} else {
  if (move_uploaded_file($_FILES["imageFile"]["tmp_name"], $target_file)) {
    echo "The file ". basename( $_FILES["imageFile"]["name"]). " has been uploaded.";
  } else {
    echo "Sorry, the file ". basename( $_FILES["imageFile"]["name"]). " has not been uploaded.";
  }
}

// Remove files older than 7 day
if (is_dir($target_dir)) {
   $path = './uploads/';
   $files = glob($path.'/*.*');

   foreach($files as $file) {
      if (is_file($file)) {
         if (time() - filemtime($file) > 7 * 24 * 60 * 60) {
            echo $file;
            unlink($file);
         }
      }
   }
}

