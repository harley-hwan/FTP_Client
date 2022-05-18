var post = function (obj) {
    $(obj.onLoad);
    return obj;
}({
    onLoad: function () {
        var modal = document.getElementById('myModal');
        $("img").on("click", function () {
            $("#ImgModal").show();
            $("#ImgModal .modal-content").attr("src", this.src);
        });
        $("#ImgModal").on("click", function () {
            $("#ImgModal").hide();
        });
        $("#ImgModal .close").on("click", function () {
            $("#ImgModal").hide();
        });
        /*$("article img").each(function () {
            $parent = $(this).parent();
            $parent.addClass("img-area");
            $(this).before($("<div class='image-title'></div>").css("max-width", $(this).css("max-width"))
                .append($("<i class='fa fa-minus-square code-collapse' style='margin-left:10px;margin-right:10px;'></i>"))
                .append("&nbsp;[이미지 보기]"));
            $parent.addClass("image-view");
        });*/
        $("pre code.hljs").each(function () {
            $(this).before($("<a href='javascript:void(0);' class='code-copy'><i class='fa fa-copy'></i>Copy!</a>"));
            $(this).before($("<div class='code-title'></div>").append($("<i class='fa fa-minus-square code-collapse' style='margin-right:10px;'></i>"))
                .append("&nbsp;[소스 보기]&nbsp;" + $(this).data("type")));
            $(this).parent().addClass("code-view");
        });
        $(document).on("click", ".code-title", function () {
            $this = $(this);
            $i = $this.find("i.code-collapse");
            if ($i.hasClass("fa-plus-square")) {
                $i.removeClass("fa-plus-square");
                $i.addClass("fa-minus-square");
                $this.parent().removeClass("code-view-disabled");
            } else {
                $i.removeClass("fa-minus-square");
                $i.addClass("fa-plus-square");
                $this.parent().addClass("code-view-disabled");
            }
        });
        $(document).on("click",'.code-copy', function(){
            $parent = $(this).closest(".code-view");
            if($parent.hasClass("code-view-disabled")){
                $i = $parent.find("i.code-collapse");
                $i.removeClass("fa-plus-square");
                $i.addClass("fa-minus-square");
                $parent.removeClass("code-view-disabled");
            }
            toastr.success(null,"소스가 복사되었습니다.", {timeOut: 700});
            var code_element = $(this).closest("pre").find("code")[0];
            var value = code_element.innerText.replace(/\n\n\n/ig, '').replace('    \n','');
            var selection = window.getSelection();
            var body_element = document.getElementsByTagName('body')[0];
            var newdiv = document.createElement('div');
            newdiv.style.position = 'absolute';
            newdiv.style.left = '-10000px';
            newdiv.style.top = '-10000px';
            body_element.appendChild(newdiv);
            newdiv.innerHTML = "<pre>" + escapeHTML(value) + "</pre>";
            selection.selectAllChildren(newdiv);
            setTimeout(function(){
                newdiv.remove();
            },10000);
            document.execCommand('copy');
            
        });
        /*$(document).on("click", ".image-title", function () {
            $this = $(this);
            $i = $this.find("i.code-collapse");
            if ($i.hasClass("fa-plus-square")) {
                $i.removeClass("fa-plus-square");
                $i.addClass("fa-minus-square");
                $this.parent().removeClass("image-view-disabled");
            } else {
                $i.removeClass("fa-minus-square");
                $i.addClass("fa-plus-square");
                $this.parent().addClass("image-view-disabled");
            }
        });*/
    }
});