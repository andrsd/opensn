from docutils import nodes
from docutils.parsers.rst import Directive, directives
import json
import os

class ParametersDirective(Directive):
    has_content = True
    required_arguments = 1

    def run(self):
        config = self.state.document.settings.env.config
        file_path = config.parameters_json_file
        class_name = self.arguments[0]

        if not os.path.isfile(file_path):
            error = self.state_machine.reporter.error(
                f'File not found: {file_path}', nodes.literal_block(self.block_text, self.block_text), line=self.lineno)
            return [error]

        with open(file_path, 'r') as file:
            data = json.load(file)

        class_data = next((cls for cls in data if cls['name'] == class_name), None)
        if class_data is None:
            error = self.state_machine.reporter.error(
                f'Class {class_name} not found in {file_path}', nodes.literal_block(self.block_text, self.block_text), line=self.lineno)
            return [error]

        parameters = class_data['parameters']
        required = [param for param in parameters if param['required'] == 1]
        optional = [param for param in parameters if param['required'] == 0]

        param_list = nodes.bullet_list()

        if required:
            it = nodes.list_item('', nodes.strong(text='Required:'))
            bullet_list = nodes.bullet_list()
            for param in required:
                item = nodes.list_item()
                para = nodes.paragraph()
                para += nodes.strong(text=f"{param['name']} ({param['type']}) - ")
                para += nodes.Text(param['description'])
                item += para
                bullet_list += item
            it += bullet_list
            param_list += it
        if optional:
            it = nodes.list_item('', nodes.strong(text='Optional:'))
            bullet_list = nodes.bullet_list()
            for param in optional:
                item = nodes.list_item()
                para = nodes.paragraph()
                para += nodes.strong(text=f"{param['name']} ({param['type']})")
                if 'default_value' in param:
                    para += nodes.Text(f", default: {param['default_value']}")
                para += nodes.Text(" - ")
                para += nodes.Text(param['description'])
                item += para
                bullet_list += item
            it += bullet_list
            param_list += it

        return [param_list]

def setup(app):
    app.add_directive('parameters', ParametersDirective)
    app.add_config_value('parameters_json_file', '', 'env')
