import os
import time
import yaml
from selenium import webdriver


def up_hexo() -> bool:
    if os.system("hexo g -d") != 0:
        print("自动部署失败")
        return False
    print("自动部署成功")

    # 打开gitee文件
    giteeFile = open("giteeinf.yml", encoding="UTF-8")

    if giteeFile is not None:
        # 加载gitee配置信息
        giteeInf = yaml.load(giteeFile, Loader=yaml.FullLoader)

        giteeUsername = giteeInf["gitee-username"]
        giteePassword = giteeInf["gitee-password"]

        webb = webdriver.Chrome()
        webb.get("https://gitee.com/login")
        # time.seep(5)

        from selenium.webdriver.common.by import By
        webb.find_element(By.ID, "user_login").send_keys(giteeUsername)
        webb.find_element(By.ID, "user_password").send_keys(giteePassword)
        webb.find_element(By.XPATH,"/html/body/div[2]/div[2]/div/div[1]/div[2]/div[1]/form[1]/div/div/div/div[4]/input").click()

        time.sleep(0.5)
        webb.get("https://gitee.com/" + giteeUsername + "/" + giteeUsername + "/pages")
        time.sleep(3)
        webb.find_element(By.XPATH, "//div[@class='button orange redeploy-button ui update_deploy']").click()
        time.sleep(0.5)

        alert = webb.switch_to.alert  # 创建弹窗对象

        alert.accept()  # 点击弹窗中的【确定】
        input('wait...')
        # 已开启 Gitee Pages 服务，网站地址

    else:
        print("未配置gitee配置信息")

    return True


up_hexo()
