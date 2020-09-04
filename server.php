<?php
    echo "데이터 파일의 경로를 입력하세요.\n";
    $name = trim(fgets(STDIN));


    function array_name_search($arr, $name){
        foreach($arr as $idx => $item)
            if($item->name === $name)
                return $idx;
        return -1;
    }

    $dataList = [];
    $database = file_get_contents("./database.txt");

    foreach(explode("\n", $database) as $data){
        if(strpos($data, " ")){
            $split = explode(" ", $data);
            $dataList[] = (object)['name'=> $split[0], 'value' => $split[1]];
        }
    }
    

    $read = file_get_contents("./data.txt");

    /**
     * Data Input & Sum
     */
    foreach(explode("\n", $read) as $data)
    {
        if(strpos($data, " ")){
            $split = explode(" ", $data);
            $idx = array_name_search($dataList, $split[0]);
            if($idx >= 0) $dataList[$idx]->value += (int)$split[1];
            else array_push($dataList, (object)["name" => $split[0], "value" => (int)$split[1]]);
        }
    }
    
    /**
     * Sort
     */
    for($i = 0; $i < count($dataList); $i++){
        for($j = $i + 1; $j < count($dataList); $j++){
            if( $dataList[$i]->value < $dataList[$j]->value ){
                $temp = $dataList[$i];
                $dataList[$i] = $dataList[$j];
                $dataList[$j] = $temp;
            }
        }
    }

    /**
     * Save
     */
    $saveData = "";
    
    foreach($dataList as $data){
        $saveData .= "{$data->name} {$data->value}\n";
    }
    $saveData = mb_substr($saveData, 0, -1);
    

    file_put_contents("database.txt", $saveData);