import os
import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin

def get_webpage_content(url):
    # 发送HTTP请求
    response = requests.get(url)

    # 检查请求是否成功
    if response.status_code == 200:
        # 使用BeautifulSoup解析HTML，指定GBK编码
        soup = BeautifulSoup(response.content, 'html.parser', from_encoding='gbk')

        # 提取网页正文
        webpage_content = soup.get_text()

        return webpage_content
    else:
        print("Failed to retrieve webpage")
        return None

def download_images(url):
    # 发送HTTP请求
    response = requests.get(url)

    # 检查请求是否成功
    if response.status_code == 200:
        # 使用BeautifulSoup解析HTML，指定GBK编码
        soup = BeautifulSoup(response.content, 'html.parser', from_encoding='gbk')

        # 提取所有图片链接
        img_links = [urljoin(url, img["src"]) for img in soup.find_all("img")]

        # 创建一个名为img的文件夹用于保存图片
        os.makedirs("img", exist_ok=True)

        # 下载并保存图片到img文件夹
        for img_link in img_links:
            img_filename = img_link.split("/")[-1]
            img_path = os.path.join("img", img_filename)

            # 下载图片并保存到img文件夹
            with open(img_path, "wb") as img_file:
                img_response = requests.get(img_link)
                img_file.write(img_response.content)

def main():
    url = "http://www.people.com.cn/"

    # 获取网页正文内容
    webpage_content = get_webpage_content(url)

    if webpage_content:
        # 将内容保存到文件
        with open("people_webpage_content.txt", "w", encoding="utf-8") as file:
            file.write(webpage_content)

    # 下载并保存图片
    download_images(url)

if __name__ == "__main__":
    main()
